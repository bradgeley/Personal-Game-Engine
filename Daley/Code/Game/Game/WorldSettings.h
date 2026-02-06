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
    constexpr int   s_worldSizePowerOfTwo                   = 7;        // In tiles
    constexpr float s_tileWidth                             = 1.f;      // In world units

	constexpr uint8_t s_maxOutdoorLighting                  = 15;       // 4 bits (0-15)
	constexpr uint8_t s_maxIndoorLighting                   = 15;       // 4 bits (0-15)

    //----------------------------------------------------------------------------------------------------------------------
    // Derived Constants
    //
    constexpr int   s_numTilesInRow         = (1 << s_worldSizePowerOfTwo);
    constexpr int   s_numTiles              = s_numTilesInRow * s_numTilesInRow;
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

    //----------------------------------------------------------------------------------------------------------------------
    // Static Asserts
    //
    static_assert(s_tileWidth > 0.f && s_tileWidth < 999.f, "s_tileWidth must be greater than 0 and less than 999");
}



//----------------------------------------------------------------------------------------------------------------------
struct WorldSettings
{
    //----------------------------------------------------------------------------------------------------------------------
    // Collision Settings

    float   m_entityWallBuffer              = 0.01f;
    float   m_collisionHashWiggleRoom       = 0.25f;      // Setting to 0 can create issues with multiple collisions in the same frame, too big will cost more performance.
};


