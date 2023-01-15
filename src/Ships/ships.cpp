#include "ships.h"
#include <vector>
#include <iostream>

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

void Ship::buildHull(int pointOfOriginTileID)
{
    this->mIsDestroyed = false;
    switch (mDirection)
    {
    case 0: // Up
        for (auto &mHullElement : mHull)
        {
            mHullElement.mDirection = mDirection;
            mHullElement.tileId = pointOfOriginTileID;
            pointOfOriginTileID -= Logic::battlefieldSize;
        }
        break;
    case 2: // Down
        for (auto &mHullElement : mHull)
        {
            mHullElement.mDirection = mDirection;
            mHullElement.tileId = pointOfOriginTileID;
            pointOfOriginTileID += Logic::battlefieldSize;
        }
        break;
    case 3: // Left
        for (auto &mHullElement : mHull)
        {
            mHullElement.mDirection = mDirection;
            mHullElement.tileId = pointOfOriginTileID--;
        }
        break;
    case 1: // Right
        for (auto &mHullElement : mHull)
        {
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