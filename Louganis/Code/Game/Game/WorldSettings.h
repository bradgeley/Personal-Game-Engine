// Bradley Christensen - 2022-2025
#pragma once
#include <cstddef>
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
// Static Settings
//
namespace StaticWorldSettings
{
    constexpr int   s_worldChunkSizePowerOfTwo              = 4;        // In tiles
    constexpr float s_tileWidth                             = 2.f;      // In world units

#if defined(_DEBUG)
    constexpr float s_defaultChunkLoadRadius                = 64.f;
    constexpr float s_defaultChunkUnloadRadius              = 75.f;
    constexpr float s_collisionHashRadius                   = 64.f;
    constexpr float s_flowFieldGenerationRadius             = 64.f;
    constexpr int   s_maxNumChunksToLoadPerFrame            = 5;
#else
    constexpr float s_defaultChunkLoadRadius                = 500.f;
    constexpr float s_defaultChunkUnloadRadius              = 550.f;
    constexpr float s_collisionHashRadius                   = 50.f;
    constexpr float s_flowFieldGenerationRadius             = 50.f;
    constexpr int   s_maxNumChunksToLoadPerFrame            = 10;
#endif // _DEBUG

    constexpr float s_flowFieldGenerationRadiusSquared      = s_flowFieldGenerationRadius * s_flowFieldGenerationRadius;
    constexpr float s_collisionHashRadiusSquared            = s_collisionHashRadius * s_collisionHashRadius;

    //----------------------------------------------------------------------------------------------------------------------
    // Derived Constants
    //
    constexpr int   s_numTilesInRow         = (1 << s_worldChunkSizePowerOfTwo);
    constexpr int   s_numTilesInRowMinusOne = s_numTilesInRow - 1;
    constexpr float s_numTilesInRowF        = static_cast<float>(s_numTilesInRow);
    constexpr int   s_numTilesInChunk       = (s_numTilesInRow * s_numTilesInRow);
    constexpr float s_oneOverNumTilesInRow  = 1.f / static_cast<float>(s_numTilesInRow);
    constexpr float s_tileHalfWidth         = s_tileWidth * 0.5f;
    constexpr float s_oneOverTileWidth      = 1.f / s_tileWidth;
    constexpr float s_chunkWidth            = s_tileWidth * s_numTilesInRowF;
    constexpr float s_oneOverChunkWidth     = 1.f / s_chunkWidth;
    constexpr float s_chunkHalfWidth        = s_chunkWidth * 0.5f;

    //----------------------------------------------------------------------------------------------------------------------
    // Static Asserts
    //
    static_assert(s_worldChunkSizePowerOfTwo > 0 && s_worldChunkSizePowerOfTwo < 8, "s_worldChunkSizePowerOfTwo must be between 1 and 7");
    static_assert(s_tileWidth > 0.f && s_tileWidth < 999.f, "s_tileWidth must be greater than 0 and less than 999");
}



//----------------------------------------------------------------------------------------------------------------------
struct WorldSettings
{
    bool    m_randomWorldSeed                   = true;
    size_t  m_worldSeed                         = 69;
    float   m_chunkLoadRadius                   = StaticWorldSettings::s_defaultChunkLoadRadius;
    float   m_chunkUnloadRadius                 = StaticWorldSettings::s_defaultChunkUnloadRadius;
    float   m_entityWallBuffer                  = 0.01f;
    float   m_collisionHashWiggleRoom           = 0.25f;      // Setting to 0 can create issues with multiple collisions in the same frame, too big will cost more performance.

    //----------------------------------------------------------------------------------------------------------------------
    // Generation Settings

    float m_terrainHeightOffsetScale            = 100.f;

    // Biomes
	float m_humidityScale                       = 20000.f;
	float m_mountainnessScale                   = 500.f;
	float m_oceannessScale                      = 1000.f;
    float m_rivernessScale                      = 4000.f;
    float m_forestnessScale                     = 5000.f;
    float m_treeBaseScale                       = 10.f;
	float m_islandnessScale                     = 20000.f;
	float m_temperatureScale                    = 20000.f;

    float m_coldThreshold                       = 0.2f;
    float m_forestThreshold                     = 0.5f;
    float m_deepForestThreshold                 = 0.75f;
	float m_desertHumidityThreshold             = 0.1f;

    // Ocean settings
	float m_oceanSandThreshold                  = 0.72f;
	float m_oceanShallowWaterThreshold          = 0.75f;
	float m_oceanWaterThreshold                 = 0.76f;
	float m_oceanDeepWaterThreshold             = 0.77f;

    // River settings
    float m_riverThreshold                      = 0.01f;
    float m_riverMaxDepth                       = 0.25f;
    float m_desertRiverMaxDepth                 = 0.35f;
    float m_riverToOceanTransitionSpeed         = 10.f;
    float m_riverToDesertTransitionSpeed        = 10.f;

    // Island settings
	float m_islandThreshold                     = 0.75f;  

    // Forest settings
	float m_desertTreeMultiplier                = 0.1f;    
};


