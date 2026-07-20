// Bradley Christensen - 2022-2026
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



class AssetManager;
class Chunk;
class Renderer;
class RandomNumberGenerator;
class VertexBuffer;
struct SCEntityFactory;
struct SystemContext;
struct TowerPlacementInfo;



//----------------------------------------------------------------------------------------------------------------------
class SCWorld
{
public:

    void GenerateVBO(Renderer& renderer, AssetManager& assetManager);
    void GenerateLightmap(Renderer& renderer);

    void Shutdown();

    bool IsTileSolid(int tileIndex) const;
    bool IsTileSolid(IntVec2 const& worldCoords) const;

	bool IsTileVisible(int tileIndex) const;
	bool IsTileVisible(IntVec2 const& worldCoords) const;

    bool IsTileOnPath(int tileIndex) const;
    bool IsTileOnPath(IntVec2 const& worldCoords) const;
    bool IsLocationOnPath(Vec2 const& worldPos) const;

	bool IsTileInGoal(int tileIndex) const;
	bool IsTileInGoal(IntVec2 const& worldCoords) const;
    bool IsLocationInGoal(Vec2 const& worldPos) const;

    bool DoesTileMatchTagQuery(IntVec2 const& worldCoords, TagQuery const& tagQuery) const;
    bool DoesTileMatchTagQuery(Vec2 const& worldPos, TagQuery const& tagQuery) const;

	bool DoTilesInRegionMatchQuery(IntVec2 const& bottomLeftTileCoords, IntVec2 const& topRightTileCoords, TagQuery const& tagQuery) const;

    void CacheValidSpawnLocations();
    Vec2 GetRandomSpawnLocation(RandomNumberGenerator& rng) const;

    uint8_t GetTileCost(int tileIndex) const;

	bool SetTile(IntVec2 const& tileCoords, Tile const& tile);
	bool SetTile(int tileIndex, Tile const& tile);

    // For each func - return false means stop iterating, true means keep iterating.
    void ForEachVisibleTile(const std::function<bool(IntVec2 const&, int)>& func) const;

    void ForEachPlayableTile(const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachPlayableTileOverlappingCapsule(Vec2 const& start, Vec2 const& end, float radius, const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachPlayableTileOverlappingCircle(Vec2 const& pos, float radius, const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachPlayableTileOverlappingCircle(Vec2 const& pos, float radius, TagQuery tileTagQuery, const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachPathTileOverlappingCircle(Vec2 const& pos, float radius, const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachPathTileInRange(Vec2 const& pos, float minRadius, float maxRadius, const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachPlayableTileOverlappingAABB(AABB2 const& aabb, const std::function<bool(IntVec2 const&)>& func) const;
	void ForEachPlayableTileInRegion(IntVec2 const& bottomLeftTileCoords, IntVec2 const& topRightTileCoords, const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachPlayableEdgeTile(const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachCachedEdgePathTile(const std::function<bool(IntVec2 const&)>& func) const;

	bool IsPointInsideSolidTile(Vec2 const& worldPos) const;
    int GetTileIndexAtWorldPos(Vec2 const& worldPos) const;
    Tile const* GetTileAtWorldPos(Vec2 const& worldPos) const;
    Tile* GetTileAtWorldPos(Vec2 const& worldPos);

    // Static World Functions
    static AABB2 GetVisibleWorldBounds();
    static AABB2 GetPlayableWorldBounds();

    static IntVec2 GetVisibleWorldRelativeCoords(IntVec2 const& worldCoords); // Returns relative coords where 0,0 is bottom left of visible world
    static IntVec2 GetTileCoordsAtWorldPos(Vec2 const& worldPos);
    static IntVec2 GetTileCoordsAtWorldPosClamped(Vec2 const& worldPos);
    static IntVec2 GetTileCoordsAtPlayableWorldPosClamped(Vec2 const& worldPos); // Clamps world pos to playable world bounds, and returns tile coords

    static IntVec2 GetTileIntersectionCoordsAtWorldPos(Vec2 const& worldPos);
    static Vec2 GetWorldPosAtTileIntersectionCoords(IntVec2 const& intersectionCoords);

    static AABB2 GetTileBoundsAtWorldPos(Vec2 const& worldPos);
    static AABB2 GetTileBounds(IntVec2 const& tileCoords);
    static Vec2 GetTileCenter(IntVec2 const& tileCoords);

public:

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

	std::vector<IntVec2> m_cachedSpawnLocations;
};

