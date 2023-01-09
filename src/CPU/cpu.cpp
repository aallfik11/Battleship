#include "cpu.h"
#include <random>
#include "../Ships/ships.h"
#include "../Graphics/graphics.h"

CPU::CPU()
{
    mDifficulty = 100;
    mInitialTileHitId = -1;
    mLastHitTileId = -1;      // if -1 implies that no tile has been hit/last hit tile is the last one of the ship CPU was trying to destroy
    mLastHitTileRotation = 4; // if over 3 -||-
    mTargetOrientation = 0;   // 0 is none, 1 is vertical, 2 is horizontal
    std::random_device device;
    mRng = std::mt19937(device());
    // mRng.seed(200);
    mGuessTargetLocation = std::uniform_int_distribution<unsigned int>(0, ((Logic::battlefieldSize * Logic::battlefieldSize) - 1));
    mGuessTargetRotation = std::uniform_int_distribution<unsigned int>(0, 3);
    mWillCPUCheat = std::uniform_int_distribution<unsigned int>(0, mDifficulty);
    mCheaterTakeRandomShip = std::uniform_int_distribution<unsigned int>(0, 4);
    mFiftyFifty = std::uniform_int_distribution<unsigned int>(0, 1);
    mIsTargetDestroyed = true;
    this->mTargetsHit = 0;
    this->mShipCount = 0;

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
    for (int i = 0; i < (Logic::battlefieldSize * Logic::battlefieldSize); i++)
    {
        playerBoard[i].tileId = i;
        opponentBoard[i].tileId = i;
    }
    for (auto &direction : mDirectionsTried)
        direction = false;
}

void CPU::placeShips()
{
    for (auto ship : playerShips)
    {
        bool shipPlaced = false;
        int randomLocation;
        char randomRotation;
        while (!shipPlaced)
        {
            randomLocation = mGuessTargetLocation(mRng);
            randomRotation = mGuessTargetRotation(mRng);
            shipPlaced = !(Logic::checkShipPlacement(randomLocation, randomRotation, ship->mShipLength, playerShips));
        }
        ship->mDirection = randomRotation;
        ship->buildHull(randomLocation);
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

void CPU::attack()
{
    // std::cout << "\nDEBUG: CPU ATTACKING";
    mShotLanded = false;
    do
    {
        // this part is responsible for the initial hit,and whether it'll be a random square or if the CPU will cheat
        if (mIsTargetDestroyed)
        {
            // if there is no ship that the CPU is currently shooting at, it will attempt to cheat by seeing one of the players ships. The chance for the CPU cheating is dependent on the difficulty
            if (!mWillCPUCheat(mRng) && mIsTargetDestroyed)
            { // rolls until it finds a nondestroyed ship to hit
                int randomShip = mCheaterTakeRandomShip(mRng);
                while (Logic::Players.at(0)->playerShips.at(randomShip)->mIsDestroyed)
                    randomShip = mCheaterTakeRandomShip(mRng);

                // unsigned int randomShipLength = Logic::Players.at(0)->playerShips.at(randomShip)->mShipLength;
                // rolls until it finds a nondestroyed tile to hit
                mCheaterTakeRandomShipTile = std::uniform_int_distribution<unsigned int>(0, (Logic::Players.at(0)->playerShips.at(randomShip)->mShipLength) - 1);
                unsigned int randomShipTile = mCheaterTakeRandomShipTile(mRng);

                while (Logic::Players.at(0)->playerShips.at(randomShip)->mHull.at(randomShipTile).tileId == 2)
                    randomShip = mCheaterTakeRandomShip(mRng);

                // always starts with the first tile of the ship

                // don't start with first tile, this partially breaks case 4 of cpuaihowto

                // Logic::Players.at(0)->playerShips.at(randomShip)->mHull.at(randomShipTile).tileType = 2;
                // Logic::Players.at(0)->playerShips.at(randomShip)->mHull.at(randomShipTile).mDestroyed = true;
                // Logic::Players.at(0)->playerBoard.at(Logic::Players.at(0)->playerShips.at(randomShip)->mHull.at(randomShipTile).tileId).tileType = 2;
                Logic::checkHit(Logic::Players.at(0)->playerShips.at(randomShip)->mHull.at(randomShipTile).tileId);
                mInitialTileHitId = Logic::Players.at(0)->playerShips.at(randomShip)->mHull.at(randomShipTile).tileId;
                opponentBoard.at(Logic::Players.at(0)->playerShips.at(randomShip)->mHull.at(randomShipTile).tileId).tileType = 2;
                // Logic::checkHit(0, Logic::Players.at(0)->playerShips.at(randomShip)->mHull.at(randomShipTile).tileId);
                mIsTargetDestroyed = Logic::CPUShipDestroyed;
                mTargetsHit++;
                mLastHitTileRotation = Logic::Players.at(0)->playerShips.at(randomShip)->mDirection;
                mDirectionsTried.at(mLastHitTileRotation) = true;
                mLastHitTileId = mInitialTileHitId;
                mShotLanded = true;
                // std::cout << "\nDEBUG: CHEATING";
            }
            else
            {
                unsigned int testHit = mGuessTargetLocation(mRng);

                // roll until a valid tile that hasn't been shot at is found
                while (opponentBoard.at(testHit).tileType != 0)
                    testHit = mGuessTargetLocation(mRng);
                if (Logic::checkHit(testHit))
                {
                    mIsTargetDestroyed = Logic::CPUShipDestroyed;
                    mInitialTileHitId = testHit;
                    mLastHitTileId = mInitialTileHitId;
                    opponentBoard.at(testHit).tileType = 2;
                    mTargetsHit++;
                    mShotLanded = false;
                    // if (!mIsTargetDestroyed)
                        mLastHitTileRotation = mGuessTargetRotation(mRng);
                    mDirectionsTried.at(mLastHitTileRotation) = true;
                    // std::cout << "\nDEBUG: HIT RANDOMLY";
                }
                else
                {
                    opponentBoard.at(testHit).tileType = 3;
                    mShotLanded = false;
                    // std::cout << "\nDEBUG: MISSED RANDOMLY";
                    break;
                }
            }
        }
        // mLastHitTileRotation = mGuessTargetRotation(mRng);
        else
        {
            // std::cout << "\nSHOOTING IN LINE";
            // std::cout << "\nDEBUG: last hit rotation " << (int)mLastHitTileRotation;
            // std::cout << "\nDEBUG: last hit location " << mLastHitTileId;
            // getchar();
            mShootInLine(mLastHitTileId, mLastHitTileRotation);
        }
        if (mTargetsHit == 17)
        {
            Logic::gameOver = true;
            break;
        }
        // getchar();
        GameScreen::drawGameScreen(Logic::Players[0]);

    } while (mShotLanded && !Logic::gameOver);
}

char CPU::mDetermineTargetOrientation()
{
    // mGuessTargetRotation = std::uniform_int_distribution<>(0, 1);
    char randomRotation = mGuessTargetRotation(mRng);
    bool invalidGuess = true;
    unsigned int theoreticalShot;
    while (invalidGuess)
        switch (randomRotation)
        {
        case 0:
            theoreticalShot = mInitialTileHitId - Logic::battlefieldSize;
            if (theoreticalShot < ((Logic::battlefieldSize * Logic::battlefieldSize))) // check if the shot is within the bounds of the battlefield
                                                                                       // if(opponentBoard.at(theoreticalShot).tileType == 2 &&
                break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        }
}

char CPU::mShootInLine(int lastHitId, char currentDirection)
{
    unsigned int theoreticalShot;
    switch (currentDirection)
    {
    case 0:
        theoreticalShot = mLastHitTileId - Logic::battlefieldSize;
        if (Logic::checkOutOfBounds(theoreticalShot, mLastHitTileRotation))
        {
            // mDirectionsTried.at(0) = true;
            // if (mDirectionsTried.at(2) == false)
            // {
            //     mLastHitTileRotation = 2;
            //     mDirectionsTried.at(2) = true;
            // }
            // // ugly, I currently don't know how to approach it better, though
            // else if (mDirectionsTried.at(1) == false)
            // {
            //     mLastHitTileRotation = 1;
            //     mDirectionsTried.at(1) = true;
            // }
            // else
            // {
            //     mLastHitTileRotation = 3;
            //     mDirectionsTried.at(3) = true;
            // }
            // { //if cpu gets stuck try uncommenting
            //     mLastHitTileRotation = 4;
            //     mIsTargetDestroyed = true;
            // }
            mDirectionSetup();
            mLastHitTileId = mInitialTileHitId;
            mShotLanded = true;
            return mLastHitTileRotation;
        }
        else
        { // theoreticalShot = mLastHitTileId - Logic::battlefieldSize;
            while (!Logic::checkOutOfBounds(theoreticalShot, mLastHitTileRotation) && opponentBoard.at(theoreticalShot).tileType == 2)
            {
                theoreticalShot -= Logic::battlefieldSize;
                if (opponentBoard.at(theoreticalShot).tileType == 3)
                    break;
            }
            mCheckShotVailidity(theoreticalShot);
        }
        break;
    case 1:
        theoreticalShot = mLastHitTileId + 1;
        if (Logic::checkOutOfBounds(theoreticalShot, mLastHitTileRotation))
        {
            mDirectionSetup();
            mLastHitTileId = mInitialTileHitId;
            mShotLanded = true;
        }
        else
        {
            while (!Logic::checkOutOfBounds(theoreticalShot, mLastHitTileRotation) && opponentBoard.at(theoreticalShot).tileType == 2)
            {
                theoreticalShot += 1;
                if (opponentBoard.at(theoreticalShot).tileType == 3)
                    break;
            }
            mCheckShotVailidity(theoreticalShot);
        }
        break;
    case 2:
        theoreticalShot = mLastHitTileId + Logic::battlefieldSize;
        if (Logic::checkOutOfBounds(theoreticalShot, mLastHitTileRotation))
        {
            mDirectionSetup();
            mLastHitTileId = mInitialTileHitId;
            mShotLanded = true;
        }
        else
        {
            while (!Logic::checkOutOfBounds(theoreticalShot, mLastHitTileRotation) && opponentBoard.at(theoreticalShot).tileType == 2)
            {
                theoreticalShot += Logic::battlefieldSize;
                if (opponentBoard.at(theoreticalShot).tileType == 3)
                    break;
            }
            mCheckShotVailidity(theoreticalShot);
        }
        break;
    case 3:
        theoreticalShot = mLastHitTileId - 1;
        if (Logic::checkOutOfBounds(theoreticalShot, mLastHitTileRotation))
        {
            mDirectionSetup();
            mLastHitTileId = mInitialTileHitId;
            mShotLanded = true;
        }
        else
        {
            while (!Logic::checkOutOfBounds(theoreticalShot, mLastHitTileRotation) && opponentBoard.at(theoreticalShot).tileType == 2)
            {
                theoreticalShot -= 1;
                if (opponentBoard.at(theoreticalShot).tileType == 3)
                    break;
            }
            mCheckShotVailidity(theoreticalShot);
        }
        break;
    }
}

bool CPU::mCheckShotVailidity(unsigned int theoreticalShot)
{
    if (!Logic::checkOutOfBounds(theoreticalShot, mLastHitTileRotation) && opponentBoard.at(theoreticalShot).tileType != 3)
    {
        if (Logic::checkHit(theoreticalShot))
        {
            mShotLanded = true;
            if (Logic::CPUShipDestroyed)
            {
                // std::cout << "DEBUG: PLAYER SHIP DESTROYED, ROLLING NEW TARGET" << std::endl;
                mIsTargetDestroyed = true;
                mResetDirectionsTried();
            }
            mLastHitTileId = theoreticalShot;
            opponentBoard.at(theoreticalShot).tileType = 2;
            mTargetsHit++;
        }
        else
        {
            opponentBoard.at(theoreticalShot).tileType = 3;
            mShotLanded = false;
        }
    }
    else
    {
        // std::cout << "DEBUG: DIRECTION SETUP" << std::endl
        //           << "NORTH: " << mDirectionsTried.at(0) << std::endl
        //           << "SOUTH: " << mDirectionsTried.at(2) << std::endl
        //           << "EAST: " << mDirectionsTried.at(1) << std::endl
        //           << "WEST: " << mDirectionsTried.at(3) << std::endl;
        mDirectionSetup();
        mLastHitTileId = mInitialTileHitId;
        mShotLanded = true;
    }
}

void CPU::mDirectionSetup()
{
    switch (mLastHitTileRotation)
    {
    case 0:
        if (mDirectionsTried.at(2) == false)
        {
            mDirectionsTried.at(2) = true;
            mLastHitTileRotation = 2;
        }
        else if (mDirectionsTried.at(1) == false)
        {
            mDirectionsTried.at(1) = true;
            mLastHitTileRotation = 1;
        }
        else
        {
            mDirectionsTried.at(3) = true;
            mLastHitTileRotation = 3;
        }
        break;
    case 1:
        if (mDirectionsTried.at(3) == false)
        {
            mDirectionsTried.at(3) = true;
            mLastHitTileRotation = 3;
        }
        else if (mDirectionsTried.at(0) == false)
        {
            mDirectionsTried.at(0) = true;
            mLastHitTileRotation = 0;
        }
        else
        {
            mDirectionsTried.at(2) = true;
            mLastHitTileRotation = 2;
        }
        break;
    case 2:
        if (mDirectionsTried.at(0) == false)
        {
            mDirectionsTried.at(0) = true;
            mLastHitTileRotation = 0;
        }
        else if (mDirectionsTried.at(3) == false)
        {
            mDirectionsTried.at(3) = true;
            mLastHitTileRotation = 3;
        }
        else
        {
            mDirectionsTried.at(1) = true;
            mLastHitTileRotation = 1;
        }
        break;
    case 3:
        if (mDirectionsTried.at(1) == false)
        {
            mDirectionsTried.at(1) = true;
            mLastHitTileRotation = 1;
        }
        else if (mDirectionsTried.at(2) == false)
        {
            mDirectionsTried.at(2) = true;
            mLastHitTileRotation = 2;
        }
        else
        {
            mDirectionsTried.at(0) = true;
            mLastHitTileRotation = 0;
        }
        break;
    }
}

void CPU::mResetDirectionsTried()
{
    for (auto &direction : mDirectionsTried)
        direction = false;
}