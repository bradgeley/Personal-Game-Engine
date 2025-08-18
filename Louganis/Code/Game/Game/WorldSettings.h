// Bradley Christensen - 2024
#pragma once
#include <cstdint>



constexpr uint8_t s_worldChunkSizePowerOfTwo = 3;



//----------------------------------------------------------------------------------------------------------------------
struct WorldSettings
{
    int   m_worldSeed                       = 0;
    float m_tileWidth                       = 2.f;
    float m_chunkLoadRadius                 = 1000.f;
    float m_chunkUnloadRadius               = 1100.f;
    float m_flowFieldGenerationRadius       = 50.f;
    float m_entityWallBuffer                = 0.01f;
    float m_entityMinimumMovement           = 0.0001f;
};