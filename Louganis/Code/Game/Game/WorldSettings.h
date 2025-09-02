// Bradley Christensen - 2022-2025
#pragma once
#include <cstddef>
#include <cstdint>



constexpr uint8_t s_worldChunkSizePowerOfTwo = 4;



//----------------------------------------------------------------------------------------------------------------------
struct WorldSettings
{
    bool    m_randomWorldSeed                 = true;
    size_t  m_worldSeed                       = 69;
    float   m_tileWidth                       = 2.f;
    float   m_chunkLoadRadius                 = 200.f;
    float   m_chunkUnloadRadius               = 300.f;
    float   m_flowFieldGenerationRadius       = 50.f;
    float   m_entityWallBuffer                = 0.01f;
    int     m_maxNumChunksToLoadPerFrame      = 100;
};