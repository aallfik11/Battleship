#include "graphics.h"
#include <conio.h>
#include <iostream>
#include <vector>
#include <windows.h>
// #include "cpu.h"

static const char TOP_LEFT_CORNER = 201;
static const char TOP_RIGHT_CORNER = 187;
static const char BOTTOM_LEFT_CORNER = 200;
static const char BOTTOM_RIGHT_CORNER = 188;
static const char HORIZONTAL_BORDER = 205;
static const char VERTICAL_BORDER = 186;
static const char TOP_T = 203;
static const char BOTTOM_T = 202;
static const char LEFT_T = 204;
static const char RIGHT_T = 185;
static const char CROSS = 206;
static const char WATER_HIT = 176;
static const char WHITESPACE = 32;
static const short TILE_TYPE_WATER = 0;
static const short TILE_TYPE_SHIP = 1;
static const short TILE_TYPE_SHIP_HIT = 2;
static const short TILE_TYPE_WATER_HIT = 3;
static const unsigned short CONSOLE_DEFAULT_COLOURS = 7;
static const unsigned short CONSOLE_NO_COLOURS = 0;
static const unsigned short COLOUR_WATER = 147;
static const unsigned short COLOUR_WATER_CURSOR = 154;
static const unsigned short COLOUR_SHIP = 138;
static const unsigned short COLOUR_SHIP_HIT = 76;
static const unsigned short COLOUR_WATER_HIT = 159;
static const unsigned short COLOUR_WATER_HIT_CURSOR = 156;
static const char NORTH = 0;
static const char SOUTH = 2;
static const char EAST = 1;
static const char WEST = 3;
static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

bool MainMenu::mCpu = true;
MainMenu::difficulty MainMenu::currentDifficulty = MainMenu::difficulty::normal;

void MainMenu::drawMenu()
{
    /*
    changes the size of the cmd window as well as its' font, font size and disables the blinking underscore symbol
    code spliced from a few stackoverflow posts
    */
    {
        CONSOLE_FONT_INFOEX cfi;
        cfi.cbSize = sizeof(cfi);
        cfi.nFont = 0;
        cfi.dwFontSize.X = 0;  // Width of each character in the font
        cfi.dwFontSize.Y = 30; // Height
        cfi.FontFamily = FF_DONTCARE;
        cfi.FontWeight = FW_NORMAL;
        std::wcscpy(cfi.FaceName, L"DejaVu Sans Mono"); // Choose your font
        SetCurrentConsoleFontEx(hOut, FALSE, &cfi);

        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hOut, &cursorInfo);
        cursorInfo.bVisible = false; // set the cursor visibility
        SetConsoleCursorInfo(hOut, &cursorInfo);

        COORD consoleSize;
        SMALL_RECT rect;
        rect.Top = 0;
        rect.Left = 0;
        rect.Bottom = 23;
        rect.Right = 50;
        consoleSize.X = 50;
        consoleSize.Y = 50;
        // SetConsoleScreenBufferSize(hOut, consoleSize);
        SetConsoleWindowInfo(hOut, 1, &rect);
    }

    char choice;
    bool exit;
    do
    {
        GameScreen::clearScreen();
        std::cout << "Battleship\n\n"
                  << "1. Start\n"
                  << "2. Settings\n"
                  << "0. Exit" << std::endl;
        switch (choice = getch())
        {
        case '1':
            GameScreen::clearScreen();
            startGame(mCpu);
            break;
        case '2':
            GameScreen::clearScreen();
            drawSettings();
            break;
        case '0':
            exit = true;
            break;
        }
    } while (!exit);
}

void MainMenu::startGame(bool playerVsAi)
{
    Logic::gameLoop(playerVsAi);
}

void MainMenu::drawSettings()
{
    char choice;
    bool back = false;
    while (!back)
    {
        GameScreen::clearScreen();
        std::cout << "Settings" << std::endl;
        std::cout << "1. Play with CPU: ";
        if (mCpu == true)
        {
            std::cout << "True" << std::endl;
            std::cout << "2. Difficulty: ";
            switch (currentDifficulty)
            {
            case difficulty::normal:
                std::cout << "Normal" << std::endl;
                break;
            case difficulty::hard:
                std::cout << "Hard" << std::endl;
                break;
            case difficulty::impossible:
                std::cout << "Impossible" << std::endl;
                break;
            }
        }
        else
            std::cout << "False" << std::endl;
        std::cout << "0. Back to Main Menu";
        switch (choice = getch())
        {
        case '1':
            if (mCpu == true)
                mCpu = false;
            else
                mCpu = true;
            break;
        case '2':
            switch (currentDifficulty)
            {
            case difficulty::normal:
                currentDifficulty = difficulty::hard;
                break;
            case difficulty::hard:
                currentDifficulty = difficulty::impossible;
                break;
            case difficulty::impossible:
                currentDifficulty = difficulty::normal;
                break;
            }
            break;
        case '0':
            back = true;
            break;
        }
    }
}

int GameScreen::messageCursorPositionX = 2;
int GameScreen::messageCursorPositionY = Logic::battlefieldSize + 3;
bool GameScreen::mMessageBoxNeedsCleaning = false;
int GameScreen::mLastMessageBoxLength = 0;
int GameScreen::mLastMessageBoxHeight = 0;

void GameScreen::drawShipsEditor(Player *currentPlayer)
{
    messageManager();
    setCursorPosition(1, 0);
    setConsoleColour(CONSOLE_DEFAULT_COLOURS);

    drawTile(TOP_LEFT_CORNER);
    for (int i = 0, j = 'A'; i < Logic::battlefieldSize; i++, j++)
    {
        drawTile(j);
    }
    drawTile(TOP_RIGHT_CORNER);
    endLine();

    int tileType;
    int tileId;
    int cursorPos = currentPlayer->mCursorTileID;
    for (int i = 0; i < Logic::battlefieldSize; i++)
    { // player board
        std::cout << i + 1;

        for (int j = i * Logic::battlefieldSize; j < (i + 1) * Logic::battlefieldSize; j++)
        {
            tileType = currentPlayer->playerBoard.at(j).tileType;
            tileId = currentPlayer->playerBoard.at(j).tileId;
            drawTile(tileType);
        }
        drawTile(VERTICAL_BORDER);
        if (i + 1 >= 9)
            endLine(0);
        else
            endLine();
    }

    // size 17 because that's the total amount of tiles a set of ships has
    std::vector<int> placedShipTiles;
    placedShipTiles.reserve(17);
    for (auto ship : currentPlayer->playerShips)
        if (ship->mIsDestroyed == false) // ship is destroyed by default, meaning only placed ships will get added to the vector
            for (auto shipTile : ship->mHull)
                placedShipTiles.push_back(shipTile.tileId);

    drawTile(WHITESPACE);
    drawTile(BOTTOM_LEFT_CORNER);
    drawTile(HORIZONTAL_BORDER, Logic::battlefieldSize);
    drawTile(BOTTOM_RIGHT_CORNER);
    endLine();

    setConsoleColour(CONSOLE_DEFAULT_COLOURS);

    // draw all placed ships
    for (auto shipTileId : placedShipTiles)
    {
        int cursorX = 2 + shipTileId % Logic::battlefieldSize;
        int cursorY = 1 + shipTileId / Logic::battlefieldSize;
        setCursorPosition(cursorX, cursorY);
        drawTile(TILE_TYPE_SHIP);
    }

    setCursorPosition(2 + cursorPos % Logic::battlefieldSize, 1 + cursorPos / Logic::battlefieldSize);
    // obviously, if there's no ship selected no members of a nullpointer should be accessed
    if (currentPlayer->mEditorModeCurrentSelectedShip != NULL)
    {
        bool collission = false;
        int theoreticalShipTileId = cursorPos;
        int shipLength = currentPlayer->mEditorModeCurrentSelectedShip->mShipLength;
        std::vector<int> theoreticalShipTiles(shipLength);
        int cursorX = cursorPos % Logic::battlefieldSize;
        int cursorY = cursorPos / Logic::battlefieldSize;
        switch (currentPlayer->mEditorModeCurrentSelectedShip->mDirection)
        {
        case NORTH:
            for (int i = 0; i < shipLength; i++)
            {
                for (auto shipTileId : placedShipTiles)
                {
                    if (theoreticalShipTileId == shipTileId)
                    {
                        collission = true;
                        i = shipLength; // in order to stop the outer loop
                        break;
                    }
                }
                theoreticalShipTileId -= Logic::battlefieldSize;
            }
            // sets the color depending on the ship's placement inside the board
            if (collission == true || (cursorY + 1) - shipLength < 0)
                tileType = TILE_TYPE_SHIP_HIT; // using ship hit as it's just red
            else
                tileType = TILE_TYPE_SHIP;

            for (int i = 0; i < shipLength; i++)
            {
                if (cursorY < 0)
                    break;
                drawTile(tileType, 1, true);
                setCursorPosition(currentConsoleCursorPosition().X - 1, currentConsoleCursorPosition().Y - 1);
                cursorY--;
            }

            break;
        case SOUTH:
            for (int i = 0; i < shipLength; i++)
            {
                for (auto shipTileId : placedShipTiles)
                {
                    if (theoreticalShipTileId == shipTileId)
                    {
                        collission = true;
                        i = shipLength; // in order to stop the outer loop
                        break;
                    }
                }
                theoreticalShipTileId += Logic::battlefieldSize;
            }
            // sets the color depending on the ship's placement inside the board
            if (collission == true || cursorY + shipLength > Logic::battlefieldSize)
                tileType = TILE_TYPE_SHIP_HIT; // using ship hit as it's just red
            else
                tileType = TILE_TYPE_SHIP;

            for (int i = 0; i < shipLength; i++)
            {
                if (cursorY >= Logic::battlefieldSize)
                    break;
                drawTile(tileType, 1, true);
                endLine(cursorX + 2);
                cursorY++;
            }
            break;
        case EAST:
            for (int i = 0; i < shipLength; i++)
            {
                for (auto shipTileId : placedShipTiles)
                {
                    if (theoreticalShipTileId == shipTileId)
                    {
                        collission = true;
                        i = shipLength; // in order to stop the outer loop
                        break;
                    }
                }
                theoreticalShipTileId++;
            }
            // sets the color depending on the ship's placement inside the board
            if (collission == true || cursorX + shipLength > Logic::battlefieldSize)
                tileType = TILE_TYPE_SHIP_HIT; // using ship hit as it's just red
            else
                tileType = TILE_TYPE_SHIP;

            for (int i = 0; i < shipLength; i++)
            {
                if (cursorX >= Logic::battlefieldSize)
                    break;
                drawTile(tileType, 1, true);
                cursorX++;
            }
            break;
        case WEST:
            for (int i = 0; i < shipLength; i++)
            {
                for (auto shipTileId : placedShipTiles)
                {
                    if (theoreticalShipTileId == shipTileId)
                    {
                        collission = true;
                        i = shipLength; // in order to stop the outer loop
                        break;
                    }
                }
                theoreticalShipTileId--;
            }
            // sets the color depending on the ship's placement inside the board
            if (collission == true || (cursorX + 1) - shipLength < 0)
                tileType = TILE_TYPE_SHIP_HIT; // using ship hit as it's just red
            else
                tileType = TILE_TYPE_SHIP;

            for (int i = 0; i < shipLength; i++)
            {
                if (cursorX < 0)
                    break;
                drawTile(tileType, 1, true);
                cursorX--;
                setCursorPosition(cursorX + 2, cursorY + 1);
            }
            break;
        }
    }
    setCursorPosition(2, Logic::battlefieldSize + 2);
}

void GameScreen::drawGameScreen(Player *currentPlayer)
{
    messageManager();
    setCursorPosition(1, 0);
    int tileType;
    int tileId;
    int cursorPos = currentPlayer->mCursorTileID;

    // display top border
    drawTile(TOP_LEFT_CORNER);
    for (int i = 0, j = 'A'; i <= Logic::battlefieldSize * 2; i++, j++)
    {
        if (j - 'A' == Logic::battlefieldSize)
        {
            j = 'A' - 1;
            drawTile(TOP_T);
        }
        else
            drawTile(j);
    }
    drawTile(TOP_T);
    endLine();

    // display battlefield
    for (int i = 0; i < Logic::battlefieldSize; i++)
    { // player board
        std::cout << i + 1;

        for (int j = i * Logic::battlefieldSize; j < (i + 1) * Logic::battlefieldSize; j++)
        {
            tileType = currentPlayer->playerBoard.at(j).tileType;
            drawTile(tileType);
        }
        drawTile(VERTICAL_BORDER);
        // targetting board
        for (int j = i * Logic::battlefieldSize; j < (i + 1) * Logic::battlefieldSize; j++)
        {
            tileType = currentPlayer->opponentBoard.at(j).tileType;
            tileId = currentPlayer->opponentBoard.at(j).tileId;
            if (tileId == cursorPos)
                drawTile(tileType, 1, true);
            else
                drawTile(tileType);
        }

        drawTile(VERTICAL_BORDER);
        if (i + 1 >= 9)
            endLine(0);
        else
            endLine();
    }
    // to account for endline with no offset in the previous lines
    drawTile(WHITESPACE);
    drawTile(BOTTOM_LEFT_CORNER);
    drawTile(HORIZONTAL_BORDER, Logic::battlefieldSize);
    drawTile(BOTTOM_T);
    drawTile(HORIZONTAL_BORDER, Logic::battlefieldSize);
    drawTile(BOTTOM_T);
    endLine();
    setConsoleColour(CONSOLE_DEFAULT_COLOURS);
    setCursorPosition(2, Logic::battlefieldSize + 2);
}

void GameScreen::clearScreen()
{
    // code from https://stackoverflow.com/questions/34842526/update-console-without-flickering-c

    // Get the Win32 handle representing standard output.
    // This generally only has to be done once, so we make it static.
    // static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    setConsoleColour(CONSOLE_DEFAULT_COLOURS);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD topLeft = {0, 0};

    // std::cout uses a buffer to batch writes to the underlying console.
    // We need to flush that to the console because we're circumventing
    // std::cout entirely; after we clear the console, we don't want
    // stale buffered text to randomly be written out.
    std::cout.flush();

    // Figure out the current width and height of the console window
    if (!GetConsoleScreenBufferInfo(hOut, &csbi))
    {
        // TODO: Handle failure!
        abort();
    }
    DWORD length = csbi.dwSize.X * csbi.dwSize.Y;

    DWORD written;

    // Flood-fill the console with spaces to clear it
    FillConsoleOutputCharacter(hOut, TEXT(' '), length, topLeft, &written);

    // Reset the attributes of every character to the default.
    // This clears all background colour formatting, if any.
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, length, topLeft, &written);

    // Move the cursor back to the top left for the next sequence of writes
    SetConsoleCursorPosition(hOut, topLeft);
}

void GameScreen::setConsoleColour(unsigned short colour)
{
    // also from Stackoverflow
    //  static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    SetConsoleTextAttribute(hOut, colour);
}

void GameScreen::setCursorPosition(int x, int y)
{
    // also from stackoverflow
    std::cout.flush();
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(hOut, coord);
}

void GameScreen::messageManager(const char *message, bool pause)
{
    int maxBoxLenght = 0;
    int currentBoxLength = 0;
    int lineCount = 0;
    const char *messageCpy = message;

    setCursorPosition(1, Logic::battlefieldSize + 2);
    if (mMessageBoxNeedsCleaning == true)
    {
        for (int i = 0; i < mLastMessageBoxLength; i++)
        {
            drawTile(WHITESPACE, mLastMessageBoxLength);
            endLine();
        }
        mMessageBoxNeedsCleaning = false;
    }

    while (*messageCpy++ != '\0')
    {
        if (*messageCpy == '\n' || *messageCpy == '\0')
        {
            if (currentBoxLength > maxBoxLenght)
                maxBoxLenght = currentBoxLength;
            lineCount++;
            currentBoxLength = 0;
            continue;
        }
        currentBoxLength++;
    }
    mLastMessageBoxLength = maxBoxLenght + 2;
    mLastMessageBoxHeight = lineCount + 2;

    setConsoleColour(CONSOLE_DEFAULT_COLOURS);
    setCursorPosition(1, Logic::battlefieldSize + 2);
    if (maxBoxLenght != 0)
    {
        drawTile(TOP_LEFT_CORNER);
        drawTile(HORIZONTAL_BORDER, maxBoxLenght);
        drawTile(TOP_RIGHT_CORNER);
        endLine();
        // draw message box for as long as it is necessary
        for (int i = 0; i < lineCount; i++)
        {
            drawTile(VERTICAL_BORDER);
            drawTile(WHITESPACE, maxBoxLenght);
            drawTile(VERTICAL_BORDER);
            endLine();
        }
        drawTile(BOTTOM_LEFT_CORNER);
        drawTile(HORIZONTAL_BORDER, maxBoxLenght);
        drawTile(BOTTOM_RIGHT_CORNER);

        setCursorPosition(messageCursorPositionX, messageCursorPositionY);
        while (*message != '\0')
        {
            if (*message == '\n')
            {
                endLine(2);
            }
            else
                drawTile(*message);
            message++;
        }
        if (pause == true)
        {
            int temp = getch();
            if (temp == 0 || temp == -32 || temp == 224)
                getch();
        }
        mMessageBoxNeedsCleaning = true;
    }
}

void GameScreen::drawTile(const int tileType, int amount, bool drawCursor)
{
    if (amount > 0)
    {
        char tileToRender;
        switch (tileType)
        {
        case TILE_TYPE_WATER:
        {
            if (drawCursor)
            {
                setConsoleColour(COLOUR_WATER_CURSOR);
                tileToRender = CROSS;
            }
            else
            {
                setConsoleColour(COLOUR_WATER);
                tileToRender = '~';
            }
        }
        break;
        case TILE_TYPE_SHIP:
        {
            setConsoleColour(COLOUR_SHIP);
            if (drawCursor == true)
                tileToRender = CROSS;
            else
                tileToRender = WHITESPACE;
        }
        break;
        case TILE_TYPE_SHIP_HIT:
        {
            setConsoleColour(COLOUR_SHIP_HIT);
            if (drawCursor == true)
                tileToRender = CROSS;
            else
                tileToRender = WHITESPACE;
        }
        break;
        case TILE_TYPE_WATER_HIT:
        {
            if (drawCursor == true)
            {
                setConsoleColour(COLOUR_WATER_HIT_CURSOR);
                tileToRender = CROSS;
            }
            else
            {
                setConsoleColour(COLOUR_WATER_HIT);
                tileToRender = WATER_HIT;
            }
        }
        break;
        default:
        {
            setConsoleColour(CONSOLE_DEFAULT_COLOURS);
            tileToRender = tileType;
        }
        }

        while (amount--)
        {
            std::cout << tileToRender;
        }
    }
}

COORD GameScreen::currentConsoleCursorPosition()
{
    // taken from Stackoverflow and slightly modified
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    return csbi.dwCursorPosition;
}

void GameScreen::endLine(int offset)
{
    std::cout << std::endl;
    setCursorPosition(offset, currentConsoleCursorPosition().Y);
}

void GameScreen::opponentShipStatus()
{
    // drawing initial, empty box for the ship indicators to go into
    const int offset = Logic::battlefieldSize * 2 + 4;
    setCursorPosition(offset, 0);
    for (int i = 0; i < 11; i++)
        drawTile(HORIZONTAL_BORDER);
    drawTile(TOP_RIGHT_CORNER);
    endLine(offset);
    std::cout << "   SHIPS   ";
    drawTile(VERTICAL_BORDER);
    endLine(offset);
    std::cout << " DESTROYED ";
    drawTile(VERTICAL_BORDER);
    endLine(offset);
    for (int i = 0; i < Logic::battlefieldSize - 2; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            drawTile(' ');
        }
        drawTile(VERTICAL_BORDER);
        endLine(offset);
    }
    for (int i = 0; i < 11; i++)
        drawTile(HORIZONTAL_BORDER);
    drawTile(BOTTOM_RIGHT_CORNER);

    // drawing the ship indicators
    int shipOffset = offset + 1;
    for (auto const &ship : Logic::Players.at(Logic::currentIdlingPlayerId)->playerShips)
    {
        setCursorPosition(shipOffset, 4);
        short tile;
        (ship->mIsDestroyed == true) ? tile = TILE_TYPE_SHIP_HIT : tile = TILE_TYPE_SHIP;
        for (int i = 0; i < ship->mShipLength; i++)
        {
            drawTile(tile);
            endLine(shipOffset);
        };
        shipOffset += 2;
    }
}