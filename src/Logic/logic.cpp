#include "logic.h"
#include <iostream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include "../Graphics/graphics.h"
#include "../CPU/cpu.h"
static const char ARROW_KEY_UP = 72;
static const char ARROW_KEY_DOWN = 80;
static const char ARROW_KEY_LEFT = 75;
static const char ARROW_KEY_RIGHT = 77;
static const char KEY_ENTER = 13;
static const char KEY_BACKSPACE = 8;
static const char KEY_ROTATE_LEFT = 93;
static const char KEY_ROTATE_RIGHT = 91;

MapTile::MapTile()
{
    this->tileId = 0;
    this->tileType = 0;
    this->direction = 0;
}

ShipTile::ShipTile()
{
    this->tileId = 0;
    this->mDestroyed = false;
    this->tileType = 1;
    this->mDirection = 0;
}

int Logic::battlefieldSize = 10;
int Logic::totalBoardSize = 100;
bool Logic::shipDestroyed = false;
bool Logic::gameOver = false;
std::array<Player *, 2> Logic::Players = {NULL, NULL};
int Logic::currentIdlingPlayerId = 1;
int Logic::difficulty = 0;

char Logic::checkShipPlacement(const int mapTileId, const char direction, const int shipLength, const std::array<Ship *, 5> &ships)
{
    Ship tempShip;
    tempShip.setLength(shipLength);
    tempShip.mDirection = direction;
    tempShip.buildHull(mapTileId);

    // looks weird but I haven't come up with a better idea
    // checks if the ship collides with another already existing ship's placement
    for (auto ship : ships)
    {
        if (ship->mIsDestroyed == true)
            continue;
        for (auto hullTile : ship->mHull)
        {
            for (auto tempHullTile : tempShip.mHull)
                if (tempHullTile.tileId == hullTile.tileId)
                    return 1;
        }
    }

    // checks for ship out of bounds of the map;
    switch (direction)
    {
    case 0: // Up
    {
        int minTileRow = tempShip.mShipLength - 1;
        if (mapTileId / battlefieldSize < minTileRow)
            return 1;
        break;
    }
    case 2: // Down
    {
        int maxTileRow = battlefieldSize - tempShip.mShipLength;
        if (mapTileId / battlefieldSize > maxTileRow)
            return 1;
        break;
    }
    case 3: // Left
    {
        int minTileColumn = tempShip.mShipLength - 1;
        if (mapTileId % battlefieldSize < minTileColumn)
            return 1;
        break;
    }
    case 1: // Right
    {
        int maxTileColumn = battlefieldSize - tempShip.mShipLength;
        if (mapTileId % battlefieldSize > maxTileColumn)
            return 1;
        break;
    }
    }
    return 0;
}

void Logic::gameLoop(bool playerVsAi)
{
    /*
    Player 1 is always a human, which is why I create an object representing them immediately
    Player 2 can be another human or a CPU depending on the settings, thus I create a pointer and then assign it to an object of either a Player or a CPU class
    Since CPU inherits from Player, It allows me to use them interchangeably in the code of class Logic, while the classes Player and CPU themselves handle
    their specific functions.
    */
    Player Player1;
    Player *Player2;
    if (playerVsAi)
    {
        int difficulty;
        switch (MainMenu::currentDifficulty)
        {
        case MainMenu::difficulty::normal:
            difficulty = 1000;
            break;
        case MainMenu::difficulty::hard:
            difficulty = 4;
            break;
        case MainMenu::difficulty::impossible:
            difficulty = 0;
            break;
        }
        Player2 = new CPU(difficulty);
    }
    else
    {
        Player2 = new Player;
    }
    Player1.mPlayerId = 0;
    Player2->mPlayerId = 1;
    Players[0] = &Player1;
    Players[1] = Player2;
    Player1.placeShips();
    Player2->placeShips();
    GameScreen::clearScreen();
    gameOver = false;
    while (!gameOver)
    {
        currentIdlingPlayerId = 1;
        /*
        if this is not done the ship status won't display until the player actually would have destroyed a ship
        also, it would mean that player 1 would see player 2's ship status until they destroy something to update it again
        */
        GameScreen::opponentShipStatus();
        Player1.attack();
        if (!gameOver)
        {
            GameScreen::opponentShipStatus();
            currentIdlingPlayerId = 0;
            Player2->attack();
            if (gameOver)
                Player2->winner();
        }
        else
            Player1.winner();
    }
    delete Player2;
}

bool Logic::checkHit(int targetTileId)
{
    shipDestroyed = false;
    Player *opponent;
    Ship *maybeDestroyedShip;
    opponent = Players.at(currentIdlingPlayerId);
    if (opponent->playerBoard.at(targetTileId).tileType == 1)
    {
        for (auto ship : opponent->playerShips)
        {
            for (auto hullTile : ship->mHull)
            {
                if (hullTile.tileId == targetTileId)
                {
                    maybeDestroyedShip = ship;
                    ship->healthPoints--;
                    hullTile.mDestroyed = true;
                }
            }
        }
        opponent->playerBoard.at(targetTileId).tileType = 2;

        if (maybeDestroyedShip->healthPoints == 0)
        {
            shipDestroyed = true;
            maybeDestroyedShip->shipDestroyed();
        }
        return true;
    }
    else
    {
        opponent->playerBoard.at(targetTileId).tileType = 3;
        return false;
    }
}

bool Logic::checkOutOfBounds(unsigned mapTileId, char direction)
{
    unsigned int validIds = ((battlefieldSize * battlefieldSize) - 1);
    switch (direction)
    {
    case 0:
        // mapTileId -= battlefieldSize;
        if (mapTileId > validIds)
            return true;
        break;
    case 1:
        if ((mapTileId / 10) != ((mapTileId - 1) / 10))
            return true;
        break;
    case 2:
        // mapTileId += battlefieldSize;
        if (mapTileId > validIds)
            return true;
        break;
    case 3:
        if ((mapTileId / 10) != ((mapTileId + 1) / 10))
            return true;
        break;
    }
    return false;
}

Player::Player()
{
    this->mTargetsHit = 0;
    this->mEditorModeCurrentSelectedShip = NULL;
    this->mCursorTileID = 0;
    this->mCurrentRow = 0;
    this->mCurrentColumn = 0;
    this->mShipCount = 5;

    // for (auto ship : playerShips)
    //     ship = new Ship;
    playerBoard = std::vector<MapTile>(Logic::totalBoardSize);
    opponentBoard = std::vector<MapTile>(Logic::totalBoardSize);

    // for (auto &ship : playerShips)
    //     ship = new Ship;
    this->playerShips[0] = new Ship;
    this->playerShips[1] = new Ship;
    this->playerShips[2] = new Ship;
    this->playerShips[3] = new Ship;
    this->playerShips[4] = new Ship;

    this->playerShips[0]->setLength(5);
    this->playerShips[1]->setLength(4);
    this->playerShips[2]->setLength(3);
    this->playerShips[3]->setLength(3);
    this->playerShips[4]->setLength(2);

    for (int i = 0; i < Logic::totalBoardSize; i++)
    {
        playerBoard.at(i).tileId = i;
        opponentBoard.at(i).tileId = i;
    }
}

char Player::controls(Ship *ship = NULL, bool attacking = false)
{
    char input = 0;
    bool shipSelector = false;
    if (mEditorModeCurrentSelectedShip == NULL)
    {
        shipSelector = true;
    }
    if (shipSelector == false)
    {
        input = getch();
        if (input == '+')
            shipSelector = true;
    }
    if (attacking)
        shipSelector = false;
    if (!shipSelector)
    {
        if (input == 0 || input == -32 || input == 224)
        {
            input = getch();
            // in case the player wants to change the ship's location to that which would collide with its' old location
            if (ship != NULL)
                ship->deleteHull();
            switch (input)
            {
            case ARROW_KEY_UP:
                // mCursorTileID -= battlefieldSize;
                mCurrentRow--;
                if (mCurrentRow > (Logic::battlefieldSize - 1))
                    mCurrentRow = (Logic::battlefieldSize - 1);
                mCursorTileID = (mCurrentRow * Logic::battlefieldSize) + mCurrentColumn;
                break;
            case ARROW_KEY_DOWN:
                // mCursorTileID += battlefieldSize;
                mCurrentRow++;
                if (mCurrentRow > (Logic::battlefieldSize - 1))
                    mCurrentRow = 0;
                mCursorTileID = (mCurrentRow * Logic::battlefieldSize) + mCurrentColumn;
                break;
            case ARROW_KEY_LEFT:
                // mCursorTileID--;
                mCurrentColumn--;
                if (mCurrentColumn > (Logic::battlefieldSize - 1))
                    mCurrentColumn = (Logic::battlefieldSize - 1);
                mCursorTileID = (mCurrentRow * Logic::battlefieldSize) + mCurrentColumn;
                break;
            case ARROW_KEY_RIGHT:
                // mCursorTileID++;
                mCurrentColumn++;
                if (mCurrentColumn > (Logic::battlefieldSize - 1))
                    mCurrentColumn = 0;
                mCursorTileID = (mCurrentRow * Logic::battlefieldSize) + mCurrentColumn;
                break;
            }
            return 0;
        }
        else if (input == KEY_ENTER || input == KEY_ROTATE_LEFT || input == KEY_ROTATE_RIGHT)
        {
            if (!attacking && ship != NULL)
                ship->deleteHull();
            switch (input)
            {
            case KEY_ENTER:
            {
                if (attacking == true)
                    return KEY_ENTER;
                else
                    // try
                    // {
                    if (Logic::checkShipPlacement(mCursorTileID, ship->mDirection, ship->mShipLength, playerShips))
                        // throw Logic::BadShipPlacement();
                        GameScreen::messageManager("Invalid Ship Placement", true);
                    else
                        ship->buildHull(mCursorTileID);
                // }
                // catch (const std::exception &e)
                // {
                //     std::cerr << e.what() << '\n';
                // }
            }
            break;
            case KEY_ROTATE_LEFT:
            {
                if (attacking == false)
                {
                    if (ship->mDirection == 0)
                        ship->mDirection = 3;
                    else
                        ship->mDirection--;
                }
            }
            break;
            case KEY_ROTATE_RIGHT:
            {
                if (attacking == false)
                {
                    if (ship->mDirection == 3)
                        ship->mDirection = 0;
                    else
                        ship->mDirection++;
                }
            }
            break;
            }
        }
        else
        {
            if (!attacking && ship != NULL)
                ship->deleteHull();
            try
            {
                // try implementing backspace functionality
                std::string rowInput = "0";
                unsigned int rowInputInt = -1;
                std::cout << input;
                input = tolower(input);
                input -= 97;
                char input2;
                std::cin >> rowInput;

                // new system that would work with backspace, work on that later
                // do
                // {

                //     input2 = getch();
                //     if (input2 == 0 || input2 == -32 || input2 == 224)
                //         getch();
                //     if(input2 == KEY_BACKSPACE)
                //         rowInput.pop_back();
                // } while (stoi(rowInput) != 0);

                rowInputInt = stoi(rowInput);

                if (input > 25 || input >= Logic::battlefieldSize)
                {
                    GameScreen::messageManager("Invalid Column Selection");
                    return 0;
                }
                if (rowInputInt > 25 || rowInputInt >= Logic::battlefieldSize)
                {
                    GameScreen::messageManager("Invalid Row Selection");
                    return 0;
                }

                mCurrentColumn = input;
                mCurrentRow = rowInputInt;
                mCursorTileID = (mCurrentRow * Logic::battlefieldSize) + mCurrentColumn;
                // add checkshiplacement here
            }
            catch (const std::exception &e)
            {

                std::cout << e.what() << '\n'
                          << "Press Any Key to Try Again...";
                input = getch();
                if (input == 0 || input == -32 || input == 224)
                    getch();
            }
            return 0;
        }
    }
    else
    {
        char *message = "Select Ship Type:\nCarrier - Size 5 - Input 'C' or 'c'\nBattleship - Size 4 - Input 'B' or 'b'\nDestroyer - Size 3 - Input 'D' or 'd'\nSubmarine - Size 3 - Input 'S' or 's'\nPatrol Boat - Size 2 - Input 'P' or 'p'\n";
        GameScreen::messageManager(message);
        input = getch();
        if (input == 0 || input == -32 || input == 224)
            getch();
        input = tolower(input);
        switch (input)
        {
        // cbdsp
        case 'c':
            mEditorModeCurrentSelectedShip = playerShips[0];
            return 5;
            break;
        case 'b':
            mEditorModeCurrentSelectedShip = playerShips[1];
            return 4;
            break;
        case 'd':
            mEditorModeCurrentSelectedShip = playerShips[2];
            return 3;
            break;
        case 's':
            mEditorModeCurrentSelectedShip = playerShips[3];
            return 2;
            break;
        case 'p':
            mEditorModeCurrentSelectedShip = playerShips[4];
            return 1;
            break;
        default:
        {
            GameScreen::drawShipsEditor(this);
            char *message = "Invalid Ship Type Selected\n Ship Types Available:\nCarrier - Size 5 - Input 'C' or 'c'\nBattleship - Size 4 - Input 'B' or 'b'\nDestroyer - Size 3 - Input 'D' or 'd'\nDestroyer - Size 3 - Input 'D' or 'd'\nSubmarine - Size 3 - Input 'S' or 's'\nPatrol Boat - Size 2 - Input 'P' or 'p'\nPress Any Key To Try Selecting a Ship Again...";
            GameScreen::messageManager(message, true);
        }
        }
    }
    return 0;
}

void Player::placeShips()
{
    bool allPlaced = false;
    while (!allPlaced)
    {
        int allPlacedCheck = 0;

        GameScreen::drawShipsEditor(this);
        controls(mEditorModeCurrentSelectedShip);

        for (auto ship : playerShips)
            if (!ship->mIsDestroyed)
                allPlacedCheck++;
        if (allPlacedCheck == 5)
            allPlaced = true;
    }
    for (auto ship : playerShips)
    {
        for (auto hullTile : ship->mHull)
        {
            playerBoard.at(hullTile.tileId).tileType = 1;
            playerBoard.at(hullTile.tileId).direction = hullTile.direction;
        }
    }
}

void Player::attack()
{
    bool shotLanded = false;
    do
    {
        GameScreen::drawGameScreen(this);
        if (controls(NULL, true) == KEY_ENTER)
        {
            // check if target coordinates are eligible for shooting, in case it's a tile that has already been shot at, the player is prevented from choosing that tile
            if (opponentBoard.at(mCursorTileID).tileType == 2 || opponentBoard.at(mCursorTileID).tileType == 3)
            {
                GameScreen::messageManager("Target Sector Has Been Already Hit\nPress Any Key to Resume and Choose a New Target", true);

                shotLanded = true;
                continue;
            }
            // the player is allowed to attack as long as they are hitting enemy ships;
            if (Logic::checkHit(mCursorTileID))
            {
                shotLanded = true;
                if (Logic::shipDestroyed == true)
                    GameScreen::opponentShipStatus();
                opponentBoard.at(mCursorTileID).tileType = 2;
                mTargetsHit++;
            }
            else
            {
                shotLanded = false;
                opponentBoard.at(mCursorTileID).tileType = 3;
            }
            if (mTargetsHit == 17) // 17 is the total hp of all ships combined
            {
                Logic::gameOver = true;
                shotLanded = false; // in order to finish shooting, there's no point in doing that if the player has already destroyed all of the opponent's ships
            }
        }
        else
            shotLanded = true;
    } while (shotLanded);
}

void Player::winner()
{
    GameScreen::clearScreen();
    std::cout << "Player " << mPlayerId + 1 << " Wins!";
    int temp = getch();
    if (temp == 0 || temp == -32 || temp == 224)
        getch();
}

Player::~Player()
{
    delete playerShips[0];
    delete playerShips[1];
    delete playerShips[2];
    delete playerShips[3];
    delete playerShips[4];
}

/*         N
          W E
/0123456789S
A0000000010
B0000000010
C0000000010
D0000000010
E0000000000

*/