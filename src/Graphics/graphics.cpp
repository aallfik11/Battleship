#include "graphics.h"
#include <conio.h>
#include <iostream>
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
static const short TILE_TYPE_WATER = 0;
static const short TILE_TYPE_SHIP = 1;
static const short TILE_TYPE_SHIP_HIT = 2;
static const short TILE_TYPE_WATER_HIT = 3;
static const unsigned short CONSOLE_DEFAULT_COLOURS = 7;
static const unsigned short CONSOLE_NO_COLOURS = 0;
static const unsigned short COLOUR_WATER = 147;
static const unsigned short COLOUR_WATER_CURSOR = 154;
static const unsigned short COLOUR_SHIP = 136;
static const unsigned short COLOUR_SHIP_HIT = 76;
static const unsigned short COLOUR_WATER_HIT = 159;
static const unsigned short COLOUR_WATER_HIT_CURSOR = 156;
static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

bool MainMenu::mCpu = true;

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
        rect.Bottom = 19;
        rect.Right = 35;
        consoleSize.X = 50;
        consoleSize.Y = 50;
        // SetConsoleScreenBufferSize(hOut, consoleSize);
        SetConsoleWindowInfo(hOut, 1, &rect);
    }

    char choice;
    bool exit;
    do
    {
        std::cout << "Battleship\n\n"
                  << "1. Start\n"
                  << "2. Settings\n"
                  << "0. Exit" << std::endl;
        switch (choice = getch())
        {
        case '1':
            startGame(mCpu);
            break;
        case '2':
            drawSettings();
            break;
        case '0':
            exit = true;
            break;
        }
        // GameScreen::clearScreen();
    } while (!exit);
}

void MainMenu::startGame(bool playerVsAi)
{
    Logic::gameLoop(playerVsAi);
}

void MainMenu::drawSettings() {}

int GameScreen::messageCursorPositionX = 1;
int GameScreen::messageCursorPositionY = Logic::battlefieldSize + 2;

void GameScreen::drawShipsEditor(Player *currentPlayer)
{
    // int battlefieldSize = Logic::battlefieldSize;

    // // top frame of the battlefield
    // std::cout << TOP_LEFT_CORNER;
    // for (int i = 0; i < battlefieldSize; i++)
    //     std::cout << HORIZONTAL_BORDER;
    // std::cout << TOP_RIGHT_CORNER;
}

void GameScreen::drawGameScreen(Player *currentPlayer)
{ // debug only, later on will need to be made better
    // clearScreen();
    setCursorPosition(1, 0);
    // system("cls");

    // int temp = 0;
    // for (int i = 0; i < Logic::battlefieldSize * Logic::battlefieldSize; i++)
    // {
    //     if (i == currentPlayer->mCursorTileID)
    //         std::cout << 'X';
    //     else
    //         std::cout << (int)currentPlayer->opponentBoard.at(i).tileType;
    //     if ((temp + 1) % 10 == 0)
    //         std::cout << std::endl;
    //     temp++;
    // }
    // std::cout << std::endl
    //           << std::endl
    //           << "current cursor ID: " << currentPlayer->mCursorTileID;
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
            tileId = currentPlayer->playerBoard.at(j).tileId;
            drawTile(tileType);
        }
        drawTile(VERTICAL_BORDER);
        // targetting board
        for (int j = i * Logic::battlefieldSize; j < (i + 1) * Logic::battlefieldSize; j++)
        {
            tileType = currentPlayer->opponentBoard.at(j).tileType;
            tileId = currentPlayer->opponentBoard.at(j).tileId;
            if (tileId == cursorPos)
                drawTile(tileType, true);
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
    drawTile(' ');
    drawTile(LEFT_T);
    for (int i = 0; i <= Logic::battlefieldSize * 2; i++)
    {
        if (i == Logic::battlefieldSize)
            drawTile(BOTTOM_T);
        else
            drawTile(HORIZONTAL_BORDER);
    }
    drawTile(CROSS);
    endLine();

    // display empty message box
    drawTile(VERTICAL_BORDER);
    for (int i = 0; i <= Logic::battlefieldSize * 2; i++)
        drawTile(' ');
    drawTile(VERTICAL_BORDER);
    endLine();

    drawTile(BOTTOM_LEFT_CORNER);
    for (int i = 0; i <= Logic::battlefieldSize * 2; i++)
        drawTile(HORIZONTAL_BORDER);

    drawTile(BOTTOM_RIGHT_CORNER);
    endLine();

    // since I'm not using clearscreen, I need to fill any potential leftover message with whitespaces
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < Logic::totalBoardSize + 3; j++)
            std::cout << ' ';
    opponentShipStatus();
    setConsoleColour(CONSOLE_DEFAULT_COLOURS);
    setCursorPosition(2, Logic::battlefieldSize + 2);
}

void GameScreen::clearScreen()
{
    // code from https://stackoverflow.com/questions/34842526/update-console-without-flickering-c

    // Get the Win32 handle representing standard output.
    // This generally only has to be done once, so we make it static.
    // static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

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
    //  static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(hOut, coord);
}

void GameScreen::displayMessage(const char *message, unsigned short colour)
{
}

void GameScreen::drawTile(const int tileType, bool drawCursor)
{
    switch (tileType)
    {
    case TILE_TYPE_WATER:
    {
        if (drawCursor)
        {
            setConsoleColour(COLOUR_WATER_CURSOR);
            std::cout << CROSS;
        }
        else
        {
            setConsoleColour(COLOUR_WATER);
            std::cout << '~';
        }
    }
    break;
    case TILE_TYPE_SHIP:
    {
        setConsoleColour(COLOUR_SHIP);
        if (drawCursor == true)
            std::cout << CROSS;
        else
            std::cout << ' ';
    }
    break;
    case TILE_TYPE_SHIP_HIT:
    {
        setConsoleColour(COLOUR_SHIP_HIT);
        if (drawCursor == true)
            std::cout << CROSS;
        else
            std::cout << ' ';
    }
    break;
    case TILE_TYPE_WATER_HIT:
    {
        if (drawCursor == true)
        {
            setConsoleColour(COLOUR_WATER_HIT_CURSOR);
            std::cout << CROSS;
        }
        else
        {
            setConsoleColour(COLOUR_WATER_HIT);
            std::cout << WATER_HIT;
        }
    }
    break;
    default:
    {
        setConsoleColour(CONSOLE_DEFAULT_COLOURS);
        std::cout << char(tileType);
    }
    }
}

COORD GameScreen::currentConsoleCursorPosition()
{
    // taken from Stackoverflow and slightly modified
    //  static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
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