// Bradley Christensen - 2022-2023
#pragma once
#include "Vec2.h"
#include <mutex>
#include <random>



//----------------------------------------------------------------------------------------------------------------------
// THE Random Number Generator
//
extern class RandomNumberGenerator* g_rng;



//----------------------------------------------------------------------------------------------------------------------
class RandomNumberGenerator
{
public:

    explicit RandomNumberGenerator(size_t seed = 0);

    void SetSeed(size_t seed);
    size_t GenerateRandomSeed();
    unsigned int Rand(); // non-deterministic
    int PlusOrMinus(float oddsPlus = 0.5f);
    int GetRandomIntInRange(int minInclusive, int maxInclusive);
    Vec2 GetRandomVecInRange2D(Vec2 minInclusive, Vec2 maxInclusive);
    float GetRandomFloatZeroToOne();
    float GetRandomFloatInRange(float minInclusive, float maxInclusive);
    bool CoinFlip(float oddsTrue = 0.5f);
    int GetRandomInt();
    size_t GetRandomSizeT();
    
private:

    std::mutex m_randMutex;
    std::random_device m_randomDevice;

#if defined(_M_X64) || defined(_WIN64)
    std::mt19937_64 m_mersenneTwister;
#else
    std::mt19937 m_mersenneTwister;
#endif

    size_t m_seed = 0;
};