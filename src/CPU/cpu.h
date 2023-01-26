#ifndef CPU_H
#define CPU_H
#include <random>
#include <array>
#include "../Logic/logic.h"

class CPU : public Player
{
    int mDifficulty;
    std::mt19937 mRng;
    std::uniform_int_distribution<unsigned int> mGuessTargetLocation;
    std::uniform_int_distribution<unsigned int> mGuessTargetRotation;
    std::uniform_int_distribution<unsigned int> mWillCPUCheat;
    std::uniform_int_distribution<unsigned int> mCheaterTakeRandomShip;
    std::uniform_int_distribution<unsigned int> mCheaterTakeRandomShipTile;
    std::uniform_int_distribution<unsigned int> mFiftyFifty;
    unsigned int mLastHitTileId;
    unsigned int mInitialTileHitId;
    char mLastHitTileRotation;
    char mTargetOrientation;
    bool mIsTargetDestroyed;
    bool mShotLanded;
    std::array<bool, 4> mDirectionsTried;
    void mResetDirectionsTried();
    char mDetermineTargetOrientation();
    void mCheckShotVailidity(unsigned int theoreticalShot);
    void mShootInLine(int lastHitId, char currentDirection);
    void mDirectionSetup();

public:
    CPU(int difficulty);
    ~CPU();
    void placeShips();
    void attack();
    void winner();
};

#endif