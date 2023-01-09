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
static const short TILE_ID_CROSSHAIR = -1;
static const short TILE_ID_WATER = 0;
static const short TILE_ID_SHIP = 1;
static const short TILE_ID_HIT_SHIP = 2;
static const short TILE_ID_HIT_WATER = 3;
static const unsigned short CONSOLE_DEFAULT_COLOURS = 7;
static const unsigned short CONSOLE_NO_COLOURS = 0;
static const unsigned short COLOR_WATER = 147;
static const unsigned short COLOR_SHIP = 136;

bool MainMenu::mCpu = true;

void MainMenu::drawMenu()
{
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
    setCursorPosition(0, 0);
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

    for (int i = 0; i < Logic::battlefieldSize; i++)
    { // player board
        for (int j = i * Logic::battlefieldSize; j < (i + 1) * Logic::battlefieldSize; j++)
        {
            std::cout << (int)currentPlayer->playerBoard.at(j).tileType;
        }
        std::cout << ' ' << VERTICAL_BORDER << ' ';
        // targetting board
        for (int j = i * Logic::battlefieldSize; j < (i + 1) * Logic::battlefieldSize; j++)
        {
            if (j == currentPlayer->mCursorTileID)
                std::cout << 'X';
            else
                std::cout << (int)currentPlayer->opponentBoard.at(j).tileType;
        }
        std::cout << std::endl;
    }
}

void GameScreen::clearScreen()
{
    //code from https://stackoverflow.com/questions/34842526/update-console-without-flickering-c
    
    // Get the Win32 handle representing standard output.
    // This generally only has to be done once, so we make it static.
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

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
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    SetConsoleTextAttribute(hOut, colour);
}

void GameScreen::setCursorPosition(int x, int y)
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(hOut, coord);
}

void GameScreen::displayMessage(const char* message, unsigned short colour)
{

}

void GameScreen::drawTile(int tileType, bool drawCrosshair)
{
    switch (tileType)
    {
    case TILE_ID_WATER:
    {
        setConsoleColour(COLOR_WATER);
    }
    break;
    case TILE_ID_SHIP:
    {
        setConsoleColour();
    }
    break;
    case TILE_ID_HIT_SHIP:
    {
        setConsoleColour();
    }
    break;
    case TILE_ID_HIT_WATER:
    {
        setConsoleColour();
    }
    break;
    }
}