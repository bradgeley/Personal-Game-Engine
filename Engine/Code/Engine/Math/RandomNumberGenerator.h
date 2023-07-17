// Bradley Christensen - 2022-2023
#pragma once
#include <mutex>



//----------------------------------------------------------------------------------------------------------------------
// THE Random Number Generator
//
extern class RandomNumberGenerator* g_rng;



//----------------------------------------------------------------------------------------------------------------------
class RandomNumberGenerator
{
public:

    explicit RandomNumberGenerator(int seed = 0);

    void SetSeed(int seed);
    int Rand();
    int PlusOrMinus(float oddsPlus = 0.5f);
    int GetRandomIntInRange(int minInclusive, int maxInclusive);
    float GetRandomFloatZeroToOne();
    float GetRandomFloatInRange(float minInclusive, float maxInclusive);
    bool CoinFlip(float oddsTrue = 0.5f);
    
private:

    std::mutex m_randMutex;
    int m_seed = 0;
};