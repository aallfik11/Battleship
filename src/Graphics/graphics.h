#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <iostream>
#include <vector>
#include <windows.h>
#include "../Logic/logic.h"
// this will also include text-based menus until I manage to learn an actual graphics framework/library

void settings();

void clearScreen();

void setConsoleColour(unsigned short colour);

class MainMenu
{
    static bool mCpu;
    // enum class difficulty
    // {
    //     easy,
    //     medium,
    //     hard
    // };

public:
    static void startGame(bool playerVsAi);
    static void drawMenu();
    static void drawSettings();
};

class GameScreen
{
    static bool mMessageBoxNeedsCleaning;
    static int mLastMessageBoxLength;
    static int mLastMessageBoxHeight;
public:
    static int messageCursorPositionX;
    static int messageCursorPositionY;
    static void drawShipsEditor(Player *currentPlayer);
    static void drawGameScreen(Player *currentPlayer);
    static void chooseTarget(Player *currentPlayer, Player *opponentPlayer);
    static void updateScreen(Player *upperPlayer, Player *lowerPlayer);
    static void clearScreen();
    static void setCursorPosition(int x, int y);
    static void setConsoleColour(unsigned short colour);
    static void messageManager(const char *message = "", bool pause = false);
    static void drawTile(int tileType, int amount = 1, bool drawCursor = false);
    static COORD currentConsoleCursorPosition();
    static void endLine(int offset = 1);
    static void opponentShipStatus();
};

#endif /*GRAPHICS_H*/