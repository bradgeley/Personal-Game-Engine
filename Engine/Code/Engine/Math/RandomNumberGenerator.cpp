// Bradley Christensen - 2022-2023
#include "RandomNumberGenerator.h"
#include <cstdlib>



RandomNumberGenerator* g_rng = nullptr;



//----------------------------------------------------------------------------------------------------------------------
RandomNumberGenerator::RandomNumberGenerator(int seed) : m_seed(seed)
{
    srand(m_seed);
}



//----------------------------------------------------------------------------------------------------------------------
int RandomNumberGenerator::Rand()
{
    std::unique_lock lock(m_randMutex); // Makes rand() thread safe
    return rand();
}



//----------------------------------------------------------------------------------------------------------------------
int RandomNumberGenerator::GetRandomIntInRange(int minInclusive, int maxInclusive)
{
    int range = maxInclusive - minInclusive + 1; //overflow?
    return Rand() % range + minInclusive;
}



//----------------------------------------------------------------------------------------------------------------------
float RandomNumberGenerator::GetRandomFloatZeroToOne()
{
    return static_cast<float>(Rand()) / static_cast<float>(RAND_MAX);
}



//----------------------------------------------------------------------------------------------------------------------
float RandomNumberGenerator::GetRandomFloatInRange(float minInclusive, float maxInclusive)
{
    float range = maxInclusive - minInclusive;
    return GetRandomFloatZeroToOne() * range + minInclusive;
}



//----------------------------------------------------------------------------------------------------------------------
bool RandomNumberGenerator::CoinFlip(float odds)
{
    return GetRandomFloatZeroToOne() <= odds;
}