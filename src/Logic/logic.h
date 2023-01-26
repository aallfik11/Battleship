#ifndef LOGIC_H
#define LOGIC_H
#include <array>
#include <memory>
#include <string>
#include "../Ships/ships.h"


class Ship;
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
};

class Player;

class Logic
{
    static std::array<Player *, 2> Players;

public:
    static int difficulty;
    static int currentIdlingPlayerId;
    static bool gameOver;
    static bool shipDestroyed;
    static int battlefieldSize;
    static int totalBoardSize;
    static void gameLoop(bool playerVsAi);
    static char checkShipPlacement(const int mapTileId, const char direction, const int shipLength, const std::array<Ship*, 5> &ships);
    static bool checkHit(int targetTileId);
    static bool checkOutOfBounds(unsigned int mapTileId, char direction);
    friend class CPU;
    friend class GameScreen;

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
    ~Player();
    char controls(Ship *ship, bool attacking);
    virtual void placeShips();
    virtual void attack();
    virtual void winner();
    friend class GameScreen;
    friend class Logic;
    friend class Ship;
};

#endif