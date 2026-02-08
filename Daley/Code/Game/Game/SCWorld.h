// Bradley Christensen - 2022-2025
#pragma once
#include "SpawnInfo.h"
#include "WorldSettings.h"
#include "Tile.h"
#include "Engine/Assets/AssetID.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/FastGrid.h"
#include "Engine/Renderer/RendererUtils.h"
#include <map>
#include <vector>



class VertexBuffer;
class Chunk;



//----------------------------------------------------------------------------------------------------------------------
class SCWorld
{
public:

    void InitializeMap();
    void GenerateTiles(CustomWorldSettings const& settings);
    void GenerateVBO();
    void GenerateLightmap();

    void Shutdown();

    bool IsTileSolid(int tileIndex) const;
    bool IsTileSolid(IntVec2 const& worldCoords) const;

	bool SetTile(IntVec2 const& tileCoords, Tile const& tile);
	bool SetTile(int tileIndex, Tile const& tile);

    // For each func - return false means stop iterating, true means keep iterating.
    void ForEachWorldCoordsOverlappingCapsule(Vec2 const& start, Vec2 const& end, float radius, const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachWorldCoordsOverlappingCircle(Vec2 const& pos, float radius, const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachWorldCoordsOverlappingAABB(AABB2 const& aabb, const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachSolidWorldCoordsOverlappingAABB(AABB2 const& aabb, const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachSolidWorldCoordsOverlappingCapsule(Vec2 const& start, Vec2 const& end, float radius, const std::function<bool(IntVec2 const&)>& func) const;

	bool IsPointInsideSolidTile(Vec2 const& worldPos) const;

    IntVec2 GetTileCoordsAtWorldPos(Vec2 const& worldPos) const;
    int GetTileIndexAtWorldPos(Vec2 const& worldPos) const;
    Tile const* GetTileAtWorldPos(Vec2 const& worldPos) const;
    Tile* GetTileAtWorldPos(Vec2 const& worldPos);
    AABB2 GetTileBoundsAtWorldPos(Vec2 const& worldPos) const;
    AABB2 GetTileBounds(IntVec2 const& tileCoords) const;

public:

    bool m_isWorldSeedDirty                     = true; // flag that is set when the world seed changes, so we can regenerate the world
    CustomWorldSettings m_worldSettings;

    FastGrid<Tile, StaticWorldSettings::s_worldSizePowerOfTwo> m_tiles;
    int m_firstVisibleTileIndex                 = 0;
    int m_lastVisibleTileIndex                  = 0;

    bool m_isLightingDirty                      = true;
    bool m_isVBODirty                           = true;
    bool m_solidnessChanged                     = true;
    VertexBufferID m_vbo                        = RendererUtils::InvalidID;
    TextureID m_lightmap                        = RendererUtils::InvalidID; // R8G8
    #if defined(_DEBUG)
    VertexBufferID m_debugVBO                   = RendererUtils::InvalidID;
    #endif

	AssetID m_worldSpriteSheet                  = AssetID::Invalid; // cached in SRenderWorld startup

    // Tracks if the player has moved. todo: move to maybe movement component?
    bool m_playerChangedWorldCoordsThisFrame    = true;
    IntVec2 m_lastKnownPlayerWorldCoords;
};

