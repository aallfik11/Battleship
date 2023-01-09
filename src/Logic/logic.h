#ifndef LOGIC_H
#define LOGIC_H
#include <array>
#include <memory>
#include <string>
#include "../Ships/ships.h"


class Ship;

class Coordinates
{
public:
    int x;
    int y;
    Coordinates();
    Coordinates(const Coordinates &xy);
};

class MapTile
{
public:
    int tileId;
    char direction;
    char tileType;
    MapTile();
};

class ShipTile : public MapTile
{

    bool mDestroyed;
    char mDirection;

public:
    friend class Ship;
    friend class Logic;
    friend class CPU;
    ShipTile();
    // friend class Carrier;
    // friend class Battleship;
    // friend class Destroyer;
    // friend class Submarine;
    // friend class PatrolBoat;
};

class Player;

class Logic
{
    static std::array<Player *, 2> Players;

public:
    static int currentIdlingPlayerId;
    static bool gameOver;
    static bool CPUShipDestroyed;
    static int battlefieldSize;
    static void gameLoop(bool playerVsAi);
    static char checkShipPlacement(const int mapTileId, const char direction, const int shipLength, const std::array<Ship*, 5> &ships);
    static bool checkHit(int targetTileId);
    static bool checkOutOfBounds(unsigned int mapTileId, char direction);
    friend class CPU;

    class BadRow : public std::exception
    {
    public:
        const char *what() const throw()
        {
            std::string exceptionMessage ="Invalid Row Selection";
            return exceptionMessage.c_str();
        }
    };

    class BadColumn : public std::exception
    {
    public:
        const char *what() const throw()
        {
            std::string exceptionMessage ="Invalid Column Selection";
            return exceptionMessage.c_str();
        }
    };

    class BadShipPlacement : public std::exception
    {
    public:
        const char *what() const throw()
        {
            std::string exceptionMessage ="Invalid Ship Placement";
            return exceptionMessage.c_str();
        }
    };
};

class Player
{
protected:
    int mPlayerId;
    int mTargetsHit;
    unsigned int mCursorTileID;
    Ship *mEditorModeCurrentSelectedShip;
    unsigned int mMaxTile = (Logic::battlefieldSize * Logic::battlefieldSize) - 1;
    unsigned int mCurrentRow = mCursorTileID / Logic::battlefieldSize;
    unsigned int mCurrentColumn = mCursorTileID % Logic::battlefieldSize;
    bool mWinner;
    int mShipCount;

public:
    std::vector<MapTile> playerBoard;
    std::vector<MapTile> opponentBoard;
    std::array<Ship*, 5> playerShips;
    Player();
    char controls(Ship *ship, bool attacking);
    virtual void placeShips();
    virtual void attack();
    friend class GameScreen;
    friend class Logic;
    friend class Ship;
};

#endif