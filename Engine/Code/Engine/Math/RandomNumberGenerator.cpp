// Bradley Christensen - 2022-2025
#include "RandomNumberGenerator.h"
#include <cstdlib>
#include <random>



RandomNumberGenerator* g_rng = nullptr;



//----------------------------------------------------------------------------------------------------------------------
RandomNumberGenerator::RandomNumberGenerator(size_t seed) : m_seed(seed)
{
    #if defined(_M_X64) || defined(_WIN64)
        m_mersenneTwister = std::mt19937_64();
    #else
        m_mersenneTwister = std::mt19937();
    #endif

    SetSeed(seed);
}



//----------------------------------------------------------------------------------------------------------------------
void RandomNumberGenerator::SetSeed(size_t seed)
{
    std::unique_lock lock(m_randMutex);
    m_seed = seed;
    m_mersenneTwister.seed(m_seed);
}



//----------------------------------------------------------------------------------------------------------------------
size_t RandomNumberGenerator::GenerateRandomSeed()
{
    std::unique_lock lock(m_randMutex); // Makes rand() thread safe
    #if defined(_M_X64) || defined(_WIN64)
        m_seed = (static_cast<size_t>(m_randomDevice()) << 32) | static_cast<size_t>(m_randomDevice());
    #else
        m_seed = static_cast<size_t>(m_randomDevice());
    #endif
    m_mersenneTwister.seed(m_seed);
    return m_seed;
}



//----------------------------------------------------------------------------------------------------------------------
unsigned int RandomNumberGenerator::Rand()
{
    std::unique_lock lock(m_randMutex);
    return m_randomDevice();
}



//----------------------------------------------------------------------------------------------------------------------
int RandomNumberGenerator::PlusOrMinus(float oddsPlus)
{
    return CoinFlip(oddsPlus) ? 1 : -1;
}



//----------------------------------------------------------------------------------------------------------------------
int RandomNumberGenerator::GetRandomIntInRange(int minInclusive, int maxInclusive)
{
    std::uniform_int_distribution<int> dist(minInclusive, maxInclusive);

    std::unique_lock lock(m_randMutex);
    return dist(m_mersenneTwister);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 RandomNumberGenerator::GetRandomVecInRange2D(Vec2 minInclusive, Vec2 maxInclusive)
{
    std::uniform_real_distribution<float> xdist(minInclusive.x, maxInclusive.x);
    std::uniform_real_distribution<float> ydist(minInclusive.y, maxInclusive.y);

    std::unique_lock lock(m_randMutex);
    float randomX = xdist(m_mersenneTwister);
    float randomY = ydist(m_mersenneTwister);
    return Vec2(randomX, randomY);
}



//----------------------------------------------------------------------------------------------------------------------
float RandomNumberGenerator::GetRandomFloatZeroToOne()
{
    std::uniform_real_distribution<float> xdist(0.f, 1.f);

    std::unique_lock lock(m_randMutex);
    return xdist(m_mersenneTwister);
}



//----------------------------------------------------------------------------------------------------------------------
float RandomNumberGenerator::GetRandomFloatInRange(float minInclusive, float maxInclusive)
{
    std::uniform_real_distribution<float> xdist(minInclusive, maxInclusive);

    std::unique_lock lock(m_randMutex);
    return xdist(m_mersenneTwister);
}



//----------------------------------------------------------------------------------------------------------------------
bool RandomNumberGenerator::CoinFlip(float oddsTrue)
{
    return GetRandomFloatZeroToOne() <= oddsTrue;
}



//----------------------------------------------------------------------------------------------------------------------
int RandomNumberGenerator::GetRandomInt()
{
    std::uniform_int_distribution<int> dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

    std::unique_lock lock(m_randMutex);
    return dist(m_mersenneTwister);
}



//----------------------------------------------------------------------------------------------------------------------
size_t RandomNumberGenerator::GetRandomSizeT()
{
    std::uniform_int_distribution<size_t> dist(0, std::numeric_limits<size_t>::max());

    std::unique_lock lock(m_randMutex);
    return dist(m_mersenneTwister);
}