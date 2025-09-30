// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/Rgba8.h"
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
    constexpr float s_defaultChunkLoadRadius                = 200.f;
    constexpr float s_defaultChunkUnloadRadius              = 250.f;
    constexpr float s_collisionHashRadius                   = 35.f;
    constexpr float s_flowFieldGenerationRadius             = 35.f;
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

	constexpr uint8_t s_maxOutdoorLighting                  = 15;       // 4 bits (0-15)
	constexpr uint8_t s_maxIndoorLighting                   = 15;       // 4 bits (0-15)

    //----------------------------------------------------------------------------------------------------------------------
    // Derived Constants
    //
    constexpr int   s_numTilesInRow         = (1 << s_worldChunkSizePowerOfTwo);
    constexpr int   s_numTilesInRowMinusOne = s_numTilesInRow - 1;
    constexpr float s_numTilesInRowF        = static_cast<float>(s_numTilesInRow);
    constexpr int   s_numTilesInChunk       = (s_numTilesInRow * s_numTilesInRow);
    constexpr int   s_numVertsInChunk       = (6 * s_numTilesInChunk);
    constexpr float s_oneOverNumTilesInRow  = 1.f / static_cast<float>(s_numTilesInRow);
    constexpr float s_tileHalfWidth         = s_tileWidth * 0.5f;
    constexpr float s_oneOverTileWidth      = 1.f / s_tileWidth;
    constexpr float s_chunkWidth            = s_tileWidth * s_numTilesInRowF;
    constexpr float s_oneOverChunkWidth     = 1.f / s_chunkWidth;
    constexpr float s_chunkHalfWidth        = s_chunkWidth * 0.5f;

    //----------------------------------------------------------------------------------------------------------------------
    // Static Asserts
    //
    static_assert(s_worldChunkSizePowerOfTwo > 0 && s_worldChunkSizePowerOfTwo < 5, "s_worldChunkSizePowerOfTwo must be between 1 and 4");
    static_assert(s_tileWidth > 0.f && s_tileWidth < 999.f, "s_tileWidth must be greater than 0 and less than 999");
}



//----------------------------------------------------------------------------------------------------------------------
struct WorldSettings
{
    //----------------------------------------------------------------------------------------------------------------------
    // Seed

    bool    m_randomWorldSeed                   = true;
    size_t  m_worldSeed                         = 69;

    float   m_chunkLoadRadius                   = StaticWorldSettings::s_defaultChunkLoadRadius;
    float   m_chunkUnloadRadius                 = StaticWorldSettings::s_defaultChunkUnloadRadius;

    //----------------------------------------------------------------------------------------------------------------------
    // Collision Settings

    float   m_entityWallBuffer                  = 0.01f;
    float   m_collisionHashWiggleRoom           = 0.25f;      // Setting to 0 can create issues with multiple collisions in the same frame, too big will cost more performance.

    //----------------------------------------------------------------------------------------------------------------------
    // Lighting Settings

    Rgba8 m_timeOfDayTints[4]                   = { Rgba8(255, 153, 102), Rgba8::White, Rgba8(200, 102, 102), Rgba8(26, 51, 100) };    // Dawn, Day, Dusk, Night
	double m_timeOfDayDurations[4]              = { 1.0, 180.0, 1.0, 180.0 };                                                           // Dawn, Day, Dusk, Night
    double m_timeOfDayTransitionDuration        = 20.f;

    //----------------------------------------------------------------------------------------------------------------------
    // Generation Settings

    float m_terrainHeightOffsetScale            = 100.f;

    // Biomes
	float m_humidityScale                       = 2500.f;
	float m_mountainnessScale                   = 500.f;
	float m_oceannessScale                      = 1000.f;
    float m_rivernessScale                      = 5000.f;
    float m_forestnessScale                     = 2500.f;
    float m_treeBaseScale                       = 10.f;
	float m_islandnessScale                     = 5000.f;
	float m_temperatureScale                    = 2500.f;

    // Biome Detail
	int m_forestnessDetailLevel                 = 5;
	int m_temperatureDetailLevel                = 5;
	int m_rivernessDetailLevel                  = 10;
	int m_islandnessDetailLevel                 = 5;
	int m_terrainHeightDetailLevel              = 8;
	int m_humidityDetailLevel                   = 5;
	int m_mountainnessDetailLevel               = 8;
	int m_oceannessDetailLevel                  = 8;

    float m_coldThreshold                       = 0.2f;
    float m_forestThreshold                     = 0.5f;
    float m_deepForestThreshold                 = 0.75f;
	float m_desertHumidityThreshold             = 0.2f;

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


