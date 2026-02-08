// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Renderer/Rgba8.h"
#include <cstddef>
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
// Static Settings
//
namespace StaticWorldSettings
{
    constexpr int   s_worldSizePowerOfTwo   = 7;        // In tiles
    constexpr float s_tileWidth             = 1.f;      // In world units

    constexpr int   s_numWorldBoundsTiles   = 4;        // Invisible tiles off the edge of world so entities may spawn in on the edges and walk into view
    constexpr float s_visibleWorldAspect    = 2.f;      // Visible X tiles divided by visible Y tiles

	constexpr uint8_t s_maxOutdoorLighting  = 15;       // 4 bits (0-15)
	constexpr uint8_t s_maxIndoorLighting   = 15;       // 4 bits (0-15)

    //----------------------------------------------------------------------------------------------------------------------
    // Derived Constants
    //
    constexpr int   s_numTilesInRow         = (1 << s_worldSizePowerOfTwo);
    constexpr float s_worldBoundsTileWidth  = s_numWorldBoundsTiles * s_tileWidth; // e
    constexpr int   s_numTilesInRowMinusOne = s_numTilesInRow - 1;
    constexpr float s_numTilesInRowF        = static_cast<float>(s_numTilesInRow);
    constexpr int   s_numTilesInWorld       = (s_numTilesInRow * s_numTilesInRow);
    constexpr int   s_numVertsInWorld       = (6 * s_numTilesInWorld);
    constexpr float s_oneOverNumTilesInRow  = 1.f / static_cast<float>(s_numTilesInRow);
    constexpr float s_tileHalfWidth         = s_tileWidth * 0.5f;
    constexpr float s_oneOverTileWidth      = 1.f / s_tileWidth;
    constexpr float s_worldWidth            = s_tileWidth * s_numTilesInRowF;
    constexpr float s_oneOverWorldWidth     = 1.f / s_worldWidth;
    constexpr float s_worldHalfWidth        = s_worldWidth * 0.5f;

    // Visible World
    constexpr float s_numVisibleWorldTilesX = s_numTilesInRow - s_numWorldBoundsTiles;
    constexpr float s_numVisibleWorldTilesY = s_numVisibleWorldTilesX / s_visibleWorldAspect;
    constexpr float s_visibleWorldWidth     = s_numVisibleWorldTilesX * s_tileWidth;
    constexpr float s_visibleWorldHeight    = s_numVisibleWorldTilesY * s_tileWidth;
    constexpr int   s_visibleWorldBeginX    = s_numWorldBoundsTiles;
    constexpr int   s_visibleWorldEndX      = s_numTilesInRow - s_numWorldBoundsTiles - 1;
    constexpr int   s_visibleWorldBeginY    = s_numWorldBoundsTiles;
    constexpr int   s_visibleWorldEndY      = s_visibleWorldBeginY + static_cast<int>(s_numVisibleWorldTilesY) - 1;

    // World Offset (puts 0,0 in the center of the camera)
	constexpr float s_worldOffsetX          = -1.f * s_worldWidth * 0.5f;
    constexpr float s_worldOffsetY          = -1.f * (s_visibleWorldHeight) * 0.5f - s_worldBoundsTileWidth;
    
    // Camera bounds
    constexpr float s_cameraMinX            = 0.f + s_worldBoundsTileWidth + s_worldOffsetX;
    constexpr float s_cameraMaxX            = s_worldWidth - s_numWorldBoundsTiles + s_worldOffsetX;
    constexpr float s_cameraMinY            = 0.f + s_worldBoundsTileWidth + s_worldOffsetY;
    constexpr float s_cameraMaxY            = s_cameraMinY + s_visibleWorldHeight * s_tileWidth;

    // Debug
	constexpr float s_tileGridDebugDrawThickness    = 0.02f;
	constexpr float s_worldOriginDebugDrawThickness = 0.1f;

    //----------------------------------------------------------------------------------------------------------------------
    // Collision Constants
    //
    constexpr float   s_collisionEntityWallBuffer           = 0.01f;        // Minimum space between a wall and an entity after collision is resolved, in world units.
    constexpr float   s_collisionHashWiggleRoom             = 0.25f;        // Setting to 0 can create issues with multiple collisions in the same frame, too big will cost more performance.

    //----------------------------------------------------------------------------------------------------------------------
    // Static Asserts
    //
    static_assert(s_tileWidth > 0.f && s_tileWidth < 999.f, "s_tileWidth must be greater than 0 and less than 999");
}



//----------------------------------------------------------------------------------------------------------------------
struct CustomWorldSettings
{
    //----------------------------------------------------------------------------------------------------------------------
    // Collision Settings


    //----------------------------------------------------------------------------------------------------------------------
    // Generation Settings
    int     m_seed                          = 0;

    float   m_minGoalDistance               = 15.f;             // How far away is the goal from the right side of the screen, in world units
    float   m_maxGoalDistance               = 20.f;             // How far away is the goal from the right side of the screen, in world units
	int     m_goalWidth                     = 4;                // How large is the goal? In world units

	Name    m_groundTileName                = "Grass";          // The name of the tile that is the ground, as defined in TileDefs.xml
	Name    m_goalTileName                  = "IslandWater";    // The name of the tile that is the goal, as defined in TileDefs.xml
	Name    m_pathTileName                  = "ForestGrass";    // The name of the tile that is the path, as defined in TileDefs.xml
};


