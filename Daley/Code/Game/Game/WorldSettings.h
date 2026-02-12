// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Renderer/Rgba8.h"
#include <cstddef>
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
namespace StaticWorldSettings
{
    //----------------------------------------------------------------------------------------------------------------------
    // Static Settings
    //
    constexpr int   s_worldSizePowerOfTwo   = 7;        // In tiles
    constexpr float s_tileWidth             = 1.f;      // In world units

    constexpr int   s_numWorldEdgeTiles     = 4;        // Invisible tiles off the edge of world so entities may spawn in on the edges and walk into view. Each side of map has this many.
    constexpr float s_visibleWorldAspect    = 2.f;      // Visible X tiles divided by visible Y tiles

	constexpr uint8_t s_maxOutdoorLighting  = 15;       // 4 bits (0-15)
	constexpr uint8_t s_maxIndoorLighting   = 15;       // 4 bits (0-15)

    //----------------------------------------------------------------------------------------------------------------------
    // Static Collision Settings
    //
    constexpr float   s_collisionEntityWallBuffer   = 0.01f;    // Minimum space between a wall and an entity after collision is resolved, in world units.
    constexpr float   s_collisionHashWiggleRoom     = 0.25f;    // Setting to 0 can create issues with multiple collisions in the same frame, too big will cost more performance.

    //----------------------------------------------------------------------------------------------------------------------
    // Static Flow Settings
    //
    constexpr float   s_maximumFlowDistance = 999.f;    // Maximum value a tile can contain in the flow distance field

    //----------------------------------------------------------------------------------------------------------------------
    // Static Debug Settings
    //
    constexpr float s_tileGridDebugDrawThickness = 0.02f;
    constexpr float s_worldOriginDebugDrawThickness = 0.1f;


    //----------------------------------------------------------------------------------------------------------------------
    // Derived Constants
    //

    // World Tiles
    constexpr int   s_numTilesInRow         = (1 << s_worldSizePowerOfTwo);
	constexpr int   s_numTilesInWorld       = s_numTilesInRow * s_numTilesInRow;
    constexpr float s_worldEdgeTileWidth    = s_numWorldEdgeTiles * s_tileWidth;
    constexpr int   s_numTilesInRowMinusOne = s_numTilesInRow - 1;
    constexpr float s_numTilesInRowF        = static_cast<float>(s_numTilesInRow);
    constexpr int   s_numVertsInWorld       = (6 * s_numTilesInWorld);
    constexpr float s_oneOverNumTilesInRow  = 1.f / static_cast<float>(s_numTilesInRow);
    constexpr float s_tileHalfWidth         = s_tileWidth * 0.5f;
    constexpr float s_oneOverTileWidth      = 1.f / s_tileWidth;
    constexpr float s_worldWidth            = s_tileWidth * s_numTilesInRowF;
    constexpr float s_worldHeight           = s_worldWidth;
    constexpr float s_oneOverWorldWidth     = 1.f / s_worldWidth;
    constexpr float s_worldHalfWidth        = s_worldWidth * 0.5f;

    // Visible World Tiles
    constexpr float s_numVisibleWorldTilesX     = s_numTilesInRow - s_numWorldEdgeTiles * 2.f;
    constexpr float s_numVisibleWorldTilesY     = s_numVisibleWorldTilesX / s_visibleWorldAspect;
    constexpr float s_visibleWorldWidth         = s_numVisibleWorldTilesX * s_tileWidth;
    constexpr float s_visibleWorldHeight        = s_numVisibleWorldTilesY * s_tileWidth;
    constexpr int   s_visibleWorldBeginIndexX   = s_numWorldEdgeTiles;
    constexpr int   s_visibleWorldEndIndexX     = s_numTilesInRow - s_numWorldEdgeTiles - 1;
    constexpr int   s_visibleWorldBeginIndexY   = s_numWorldEdgeTiles;
    constexpr int   s_visibleWorldEndIndexY     = s_visibleWorldBeginIndexY + static_cast<int>(s_numVisibleWorldTilesY) - 1;
	constexpr int   s_numTilesInVisibleWorld    = static_cast<int>(s_numVisibleWorldTilesX * s_numVisibleWorldTilesY);
    constexpr int   s_numVertsInVisibleWorld    = (6 * s_numTilesInVisibleWorld);

    // World Positions (0,0 in the center of the camera)
    constexpr float s_worldOffsetX              = -1.f * s_worldWidth * 0.5f;
    constexpr float s_worldOffsetY              = -1.f * (s_visibleWorldHeight) * 0.5f - s_worldEdgeTileWidth;
	constexpr float s_worldMinsX                = s_worldOffsetX;
	constexpr float s_worldMaxsX                = s_worldMinsX + s_worldWidth;
	constexpr float s_worldMinsY                = s_worldOffsetY;
	constexpr float s_worldMaxsY                = s_worldMinsY + s_worldHeight;

    // Visible World Positions (Relative to 0,0 which is in the center of the camera)
    constexpr float s_visibleWorldMinsX         = -s_visibleWorldWidth * 0.5f;
    constexpr float s_visibleWorldMaxsX         = s_visibleWorldWidth * 0.5f;
    constexpr float s_visibleWorldMinsY         = -s_visibleWorldHeight * 0.5f;
    constexpr float s_visibleWorldMaxsY         = s_visibleWorldHeight * 0.5f;
    constexpr float s_visibleWorldCenterX       = (s_visibleWorldMinsX + s_visibleWorldMaxsX) * 0.5f;
    constexpr float s_visibleWorldCenterY       = (s_visibleWorldMinsY + s_visibleWorldMaxsY) * 0.5f;

    // Playable World Positions (Visible Plus Edge Tiles)
    constexpr float s_numPlayableWorldTilesX    = s_numTilesInRow;
	constexpr float s_numPlayableWorldTilesY    = s_numWorldEdgeTiles * 2 + static_cast<int>(s_numVisibleWorldTilesY);
    constexpr int   s_numTilesInPlayableWorld   = static_cast<int>(s_numPlayableWorldTilesX * s_numPlayableWorldTilesY);
    constexpr float s_playableWorldWidth        = s_numVisibleWorldTilesX * s_tileWidth;
    constexpr float s_playableWorldHeight       = s_numVisibleWorldTilesY * s_tileWidth;
	constexpr int   s_playableWorldBeginIndexX  = 0;
	constexpr int   s_playableWorldBeginIndexY  = 0;
	constexpr int   s_playableWorldEndIndexX    = static_cast<int>(s_numPlayableWorldTilesX) - 1;
	constexpr int   s_playableWorldEndIndexY    = static_cast<int>(s_numPlayableWorldTilesY) - 1;
    constexpr float s_playableWorldMinsX        = s_worldOffsetX;
    constexpr float s_playableWorldMaxsX        = s_playableWorldMinsX + s_visibleWorldWidth + s_worldEdgeTileWidth * 2.f;
    constexpr float s_playableWorldMinsY        = s_worldOffsetY;
    constexpr float s_playableWorldMaxsY        = s_playableWorldMinsY + s_visibleWorldHeight + s_worldEdgeTileWidth * 2.f;

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

    int     m_goalTileDistance              = 5;             // How far away is the goal from the left side of the screen, in tiles
	int     m_goalWidth                     = 4;                // How large is the goal? In world units

	Name    m_groundTileName                = "Grass";          // The name of the tile that is the ground, as defined in TileDefs.xml
	Name    m_goalTileName                  = "IslandWater";    // The name of the tile that is the goal, as defined in TileDefs.xml
	Name    m_pathTileName                  = "ForestGrass";    // The name of the tile that is the path, as defined in TileDefs.xml
};


