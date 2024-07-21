// Bradley Christensen - 2024
#pragma once



//----------------------------------------------------------------------------------------------------------------------
struct WorldSettings
{
    int m_worldSeed = 0;
    float m_tileWidth = 1.f;
    int m_chunkSizePowerOfTwo = 3; // if 4, 2^4 = 16 tiles wide
    float m_chunkLoadRadius = 50.f;
    float m_chunkUnloadRadius = 75.f;
};