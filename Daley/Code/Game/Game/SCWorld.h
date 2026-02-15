// Bradley Christensen - 2022-2025
#pragma once
#include "SpawnInfo.h"
#include "WorldSettings.h"
#include "Tile.h"
#include "Engine/Assets/AssetID.h"
#include "Engine/Core/TagQuery.h"
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

    void GenerateVBO();
    void GenerateLightmap();

    void Shutdown();

    bool IsTileSolid(int tileIndex) const;
    bool IsTileSolid(IntVec2 const& worldCoords) const;

	bool IsTileVisible(int tileIndex) const;
	bool IsTileVisible(IntVec2 const& worldCoords) const;

    bool IsTileOnPath(int tileIndex) const;
    bool IsTileOnPath(IntVec2 const& worldCoords) const;

    bool DoesTileMatchTagQuery(IntVec2 const& worldCoords, TagQuery const& tagQuery) const;

    void CacheValidSpawnLocations();
    Vec2 GetRandomSpawnLocation() const;

    uint8_t GetTileCost(int tileIndex) const;

	bool SetTile(IntVec2 const& tileCoords, Tile const& tile);
	bool SetTile(int tileIndex, Tile const& tile);

    // For each func - return false means stop iterating, true means keep iterating.
    void ForEachVisibleTile(const std::function<bool(IntVec2 const&, int)>& func) const;

    void ForEachPlayableTile(const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachPlayableTileOverlappingCapsule(Vec2 const& start, Vec2 const& end, float radius, const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachPlayableTileOverlappingCircle(Vec2 const& pos, float radius, const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachPlayableTileOverlappingAABB(AABB2 const& aabb, const std::function<bool(IntVec2 const&)>& func) const;

    void ForEachEdgeTile(const std::function<bool(IntVec2 const&)>& func) const;

	bool IsPointInsideSolidTile(Vec2 const& worldPos) const;

	AABB2 GetVisibleWorldBounds() const;
	AABB2 GetPlayableWorldBounds() const;

	IntVec2 GetVisibleWorldRelativeCoords(IntVec2 const& worldCoords) const; // Returns relative coords where 0,0 is bottom left of visible world
    IntVec2 GetTileCoordsAtWorldPos(Vec2 const& worldPos) const;
    IntVec2 GetTileCoordsAtWorldPosClamped(Vec2 const& worldPos) const;
    int GetTileIndexAtWorldPos(Vec2 const& worldPos) const;
    Tile const* GetTileAtWorldPos(Vec2 const& worldPos) const;
    Tile* GetTileAtWorldPos(Vec2 const& worldPos);
    AABB2 GetTileBoundsAtWorldPos(Vec2 const& worldPos) const;
    AABB2 GetTileBounds(IntVec2 const& tileCoords) const;

public:

    bool m_isWorldSeedDirty                     = true; // flag that is set when the world seed changes, so we can regenerate the world
    CustomWorldSettings m_worldSettings;

    FastGrid<Tile, StaticWorldSettings::s_worldSizePowerOfTwo> m_tiles;

    bool m_isLightingDirty                      = true;
    bool m_isVBODirty                           = true;
    bool m_solidnessChanged                     = true;
    VertexBufferID m_vbo                        = RendererUtils::InvalidID;
    TextureID m_lightmap                        = RendererUtils::InvalidID; // R8G8
    VertexBufferID m_debugVBO                   = RendererUtils::InvalidID;

	AssetID m_worldSpriteSheet                  = AssetID::Invalid; // cached in SRenderWorld startup

    // Tracks if the player has moved. todo: move to maybe movement component?
    bool m_playerChangedWorldCoordsThisFrame    = true;
    IntVec2 m_lastKnownPlayerWorldCoords;

	std::vector<IntVec2> m_spawnLocations;
};

