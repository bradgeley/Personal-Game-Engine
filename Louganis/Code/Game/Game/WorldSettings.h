// Bradley Christensen - 2022-2025
#pragma once
#include <cstdint>



constexpr uint8_t s_worldChunkSizePowerOfTwo = 3;



//----------------------------------------------------------------------------------------------------------------------
struct WorldSettings
{
    bool    m_randomWorldSeed                 = false;
    size_t  m_worldSeed                       = 69;
    float   m_tileWidth                       = 2.f;
    float   m_chunkLoadRadius                 = 250.f;
    float   m_chunkUnloadRadius               = 300.f;
    float   m_flowFieldGenerationRadius       = 50.f;
    float   m_entityWallBuffer                = 0.01f;
    float   m_entityMinimumMovement           = 0.000001f;
    int     m_maxNumChunksToLoadPerFrame      = 100;
};