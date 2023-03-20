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

    int Rand();
    int GetRandomIntInRange(int minInclusive, int maxInclusive);
    float GetRandomFloatZeroToOne();
    float GetRandomFloatInRange(float minInclusive, float maxInclusive);
    bool CoinFlip(float odds);
    
private:

    std::mutex m_randMutex;
    int m_seed = 0;
};