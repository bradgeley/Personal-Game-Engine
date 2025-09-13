// Bradley Christensen - 2022-2025
#pragma once
#include <cstddef>
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
// Static Settings
//
constexpr int s_worldChunkSizePowerOfTwo        = 4;        // In tiles
constexpr float s_tileWidth                     = 1.5f;     // In world units



//----------------------------------------------------------------------------------------------------------------------
struct WorldSettings
{
    bool    m_randomWorldSeed                   = true;
    size_t  m_worldSeed                         = 69;
    float   m_chunkLoadRadius                   = 500.f;
    float   m_chunkUnloadRadius                 = 1000.f;
    float   m_flowFieldGenerationRadius         = 50.f;
    float   m_entityWallBuffer                  = 0.01f;
    int     m_maxNumChunksToLoadPerFrame        = 100;
    float   m_collisionHashWiggleRoom           = 0.25f;      // Setting to 0 can create issues with multiple collisions in the same frame, too big will cost more performance.
};



//----------------------------------------------------------------------------------------------------------------------
// Derived Constants
//
constexpr int   s_numTilesInRow                 = (1 << s_worldChunkSizePowerOfTwo);
constexpr float s_numTilesInRowF                = static_cast<float>(s_numTilesInRow);
constexpr int   s_numTilesInChunk               = (s_numTilesInRow * s_numTilesInRow);
constexpr float s_oneOverNumTilesInRow          = 1.f / static_cast<float>(s_numTilesInRow);
constexpr float s_tileHalfWidth                 = s_tileWidth * 0.5f;
constexpr float s_oneOverTileWidth              = 1.f / s_tileWidth;
constexpr float s_chunkWidth                    = s_tileWidth * s_numTilesInRowF;
constexpr float s_chunkHalfWidth                = s_chunkWidth * 0.5f;



//----------------------------------------------------------------------------------------------------------------------
// Static Asserts
//
static_assert(s_worldChunkSizePowerOfTwo > 0 && s_worldChunkSizePowerOfTwo < 8, "s_worldChunkSizePowerOfTwo must be between 1 and 7");
static_assert(s_tileWidth > 0.f && s_tileWidth < 999.f, "s_tileWidth must be greater than 0 and less than 999");