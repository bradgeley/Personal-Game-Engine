// Bradley Christensen - 2022-2025
#pragma once
#include "WorldCoords.h"
#include "WorldSettings.h"
#include "Engine/Assets/AssetID.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/IntVec2.h"
#include <map>



class VertexBuffer;
class Chunk;



//----------------------------------------------------------------------------------------------------------------------
class SCWorld
{
public:

    Chunk* GetActiveChunk(IntVec2 const& chunkCoords) const;
    Chunk* GetActiveChunk(int chunkX, int chunkY) const;
    Chunk* GetActiveChunk(WorldCoords const& worldCoords) const;
    Chunk* GetActiveChunkAtLocation(Vec2 const& worldLocation) const;

    IntVec2 GetChunkCoordsAtLocation(Vec2 const& worldLocation) const;
    IntVec2 GetGlobalTileCoordsAtLocation(Vec2 const& worldLocation) const;
    IntVec2 GetLocalTileCoordsAtLocation(Vec2 const& worldLocation) const;
    IntVec2 GetLocalTileCoordsAtLocation(Vec2 const& worldLocation, IntVec2 const& chunkCoords) const;

    WorldCoords GetWorldCoordsAtOffset(WorldCoords const& worldCoords, IntVec2 const& tileOffset) const;
    WorldCoords GetWorldCoordsAtLocation(Vec2 const& worldLocation) const;
    WorldCoords GetWorldCoordsAtGlobalTileCoords(IntVec2 const& globalTileCoords) const;
    void GetEightNeighborWorldCoords(WorldCoords const& worldCoords, WorldCoords* eightNeighborsArray) const;

    // For each func - return false means stop iterating, true means keep iterating.
    void ForEachWorldCoordsOverlappingCapsule(Vec2 const& start, Vec2 const& end, float radius, const std::function<bool(WorldCoords const&)>& func) const;
    void ForEachWorldCoordsOverlappingCircle(Vec2 const& pos, float radius, const std::function<bool(WorldCoords const&)>& func) const;
    void ForEachWorldCoordsOverlappingAABB(AABB2 const& aabb, const std::function<bool(WorldCoords const&)>& func) const;
    void ForEachSolidWorldCoordsOverlappingAABB(AABB2 const& aabb, const std::function<bool(WorldCoords const&, Chunk*)>& func) const;
    void ForEachSolidWorldCoordsOverlappingCapsule(Vec2 const& start, Vec2 const& end, float radius, const std::function<bool(WorldCoords const&)>& func) const;
    void ForEachChunkOverlappingAABB(AABB2 const& aabb, const std::function<bool(Chunk&)>& func) const;
    void ForEachChunkCoordsOverlappingAABB(AABB2 const& aabb, const std::function<bool(IntVec2 const&)>& func) const;
    void ForEachChunkCoordsOverlappingCircle(Vec2 const& circleCenter, float circleRadius, const std::function<bool(IntVec2 const&)>& func) const;

    void GetChunksOverlappingAABB(std::vector<Chunk*>& out_chunks, AABB2 const& aabb) const;

	bool IsPointInsideSolidTile(Vec2 const& worldPos) const;

    AABB2 CalculateChunkBounds(int chunkX, int chunkY) const;
    Vec2  CalculateChunkCenter(int chunkX, int chunkY) const;
    AABB2 GetTileBoundsAtWorldPos(Vec2 const& worldPos) const;
    AABB2 GetTileBounds(WorldCoords const& worldCoords) const;
    AABB2 GetTileBounds(IntVec2 const& worldTileCoords) const;
    
    Chunk* GetOrCreateActiveChunk(int chunkX, int chunkY);
    Chunk* LoadChunk(IntVec2 const& chunkCoords);
    bool UnloadChunk(Chunk* chunk);
    bool IsChunkLoaded(IntVec2 const& chunkCoords) const;
    bool RemoveActiveChunk(IntVec2 const& coords);
    void RemoveActiveChunk(int chunkX, int chunkY);
    void RemoveActiveChunks(std::vector<IntVec2> const& coordsList);
    void ClearActiveChunks();

    float GetChunkUnloadRadius() const;

    bool GetPlayerChangedWorldCoordsThisFrame() const;

public:

    WorldSettings m_worldSettings;

    std::map<IntVec2, Chunk*> m_activeChunks;                       // Owned by SWorld

	AssetID m_worldSpriteSheet                  = AssetID::Invalid; // cached in SRenderWorld startup

    // Tracks if the player has moved. todo: move to maybe movement component?
    bool m_playerChangedWorldCoordsThisFrame    = true;
    WorldCoords m_lastKnownPlayerWorldCoords    = WorldCoords::s_invalidWorldCoords;
};

