#include "ships.h"
#include <vector>
#include <iostream>

// ShipTile::ShipTile()
// {
//     this->tileId = 0;
//     // this->TileId = 999;
//     this->mDestroyed = false;
//     this->tileType = 1;
// }

void Ship::setLength(int shipLength)
{
    mShipLength = shipLength;
    mHull.resize(mShipLength);
    healthPoints = shipLength;
}

Ship::Ship()
{
    this->mShipLength = 3;
    this->healthPoints = 0;
    this->mIsDestroyed = true;
    this->mDirection = 2;
}

// void Ship::buildHull(Coordinates *pointOfOrigin, char direction)
// {
//     bool first = true;
//     switch (direction)
//     {
//     case 'n':
//     case 'N':
//         for (auto mHullElement : mHull)
//         {
//             mHullElement.mTilePosition.x = pointOfOrigin->x;
//             mHullElement.mTilePosition.y = pointOfOrigin->y--;
//         }
//         break;
//     case 's':
//     case 'S':
//         for (auto mHullElement : mHull)
//         {
//             mHullElement.mTilePosition.x = pointOfOrigin->x;
//             mHullElement.mTilePosition.y = pointOfOrigin->y++;
//         }
//         break;
//     case 'e':
//     case 'E':
//         for (auto mHullElement : mHull)
//         {
//             mHullElement.mTilePosition.x = pointOfOrigin->x--;
//             mHullElement.mTilePosition.y = pointOfOrigin->y;
//         }
//         break;
//     case 'w':
//     case 'W':
//         for (auto mHullElement : mHull)
//         {
//             mHullElement.mTilePosition.x = pointOfOrigin->x++;
//             mHullElement.mTilePosition.y = pointOfOrigin->y;
//         }
//         break;
//     }
// }

void Ship::buildHull(int pointOfOriginTileID)
{
    this->mIsDestroyed = false;
    // bool first = true;
    switch (mDirection)
    {
    case 0: // Up
        for (auto &mHullElement : mHull)
        {
            // mHullElement.mTilePosition.x = pointOfOrigin->x;
            // mHullElement.mTilePosition.y = pointOfOrigin->y--;
            mHullElement.mDirection = mDirection;
            mHullElement.tileId = pointOfOriginTileID;
            pointOfOriginTileID -= Logic::battlefieldSize;
        }
        break;
    case 2: // Down
        for (auto &mHullElement : mHull)
        {
            // mHullElement.mTilePosition.x = pointOfOrigin->x;
            // mHullElement.mTilePosition.y = pointOfOrigin->y++;
            mHullElement.mDirection = mDirection;
            mHullElement.tileId = pointOfOriginTileID;
            pointOfOriginTileID += Logic::battlefieldSize;
        }
        break;
    case 3:
        for (auto &mHullElement : mHull)
        {
            // mHullElement.mTilePosition.x = pointOfOrigin->x--;
            // mHullElement.mTilePosition.y = pointOfOrigin->y;
            mHullElement.mDirection = mDirection;
            mHullElement.tileId = pointOfOriginTileID--;
        }
        break;
    case 1: // Right
        for (auto &mHullElement : mHull)
        {
            // mHullElement.mTilePosition.x = pointOfOrigin->x++;
            // mHullElement.mTilePosition.y = pointOfOrigin->y;
            mHullElement.mDirection = mDirection;
            mHullElement.tileId = pointOfOriginTileID++;
        }
        break;
    }
}

void Ship::deleteHull()
{
    this->mIsDestroyed = true;
    mHull.clear();
    mHull.resize(mShipLength);
}

void Ship::shipDestroyed()
{
    mIsDestroyed = true;
}