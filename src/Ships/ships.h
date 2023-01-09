#ifndef SHIPS_H
#define SHIPS_H
#include <vector>
#include <memory>
class ShipTile;
#include "../Logic/logic.h"
// class ShipTile : public MapTile{

//     bool mDestroyed;
//     char mDirection;

// public:
//     friend class Ship;
//     friend class Logic;
//     friend class CPU;
//     ShipTile();
//     // friend class Carrier;
//     // friend class Battleship;
//     // friend class Destroyer;
//     // friend class Submarine;
//     // friend class PatrolBoat;
// };

class Ship
{
    int healthPoints;
    bool mIsDestroyed;
    int mShipLength;
    char mDirection;
    std::vector<ShipTile> mHull;

public:
    Ship();
    void setLength(int shipLength);
    // void buildHull(Coordinates *pointOfOrigin, char direction);
    void buildHull(int pointOfOriginTileID);
    void deleteHull();
    void shipDestroyed();
    friend class Logic;
    friend class Player;
    friend class CPU;
};

// class Carrier : Ship
// {
//     int mShipLength = 5;
// };
// class Battleship : Ship
// {
//     int mShipLength = 4;
// };
// class Destroyer : Ship
// {
//     int mShipLength = 3;
// };
// class Submarine : Ship
// {
//     int mShipLength = 3;
// };
// class PatrolBoat : Ship
// {
//     int mShipLength = 2;
// };
#endif