// Bradley Christensen - 2024
#pragma once
#include <cstdint>



constexpr uint8_t s_worldChunkSizePowerOfTwo = 3;



//----------------------------------------------------------------------------------------------------------------------
struct WorldSettings
{
    int m_worldSeed = 0;
    float m_tileWidth = 2.f;
    float m_chunkLoadRadius = 50.f;
    float m_chunkUnloadRadius = 75.f;
};