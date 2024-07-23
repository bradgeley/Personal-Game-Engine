// Bradley Christensen - 2024
#pragma once
#include "WorldSettings.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/AABB2.h"
#include <map>
#include <vector>



class VertexBuffer;
class Chunk;



//----------------------------------------------------------------------------------------------------------------------
class SCWorld
{
public:

    Chunk* GetActiveChunk(IntVec2 const& coords) const;
    Chunk* GetActiveChunk(int x, int y) const;
    Chunk* GetActiveChunkAtLocation(Vec2 const& worldLocation) const;
    IntVec2 GetChunkCoordsAtLocation(Vec2 const& worldLocation) const;
    IntVec2 GetWorldTileCoordsAtLocation(Vec2 const& worldLocation) const;
    IntVec2 GetLocalTileCoordsAtLocation(Vec2 const& worldLocation) const;
    IntVec2 GetLocalTileCoordsAtOffset(IntVec2 const& localTileCoords, IntVec2 const& offset, IntVec2& out_chunkCoordsOffset) const;
    AABB2 CalculateChunkBounds(int x, int y) const;
    AABB2 GetTileBoundsAtWorldPos(Vec2 const& worldPos) const;
    AABB2 GetTileBounds(IntVec2 const& chunkCoords, IntVec2 const& localTileCoords) const;
    AABB2 GetTileBounds(IntVec2 const& worldTileCoords) const;
    
    Chunk* GetOrCreateActiveChunk(int x, int y);
    void RemoveActiveChunk(IntVec2 const& coords);
    void RemoveActiveChunk(int x, int y);
    void RemoveActiveChunks(std::vector<IntVec2> const& coordsList);
    void ClearActiveChunks();

    int GetNumTilesInRow() const;
    float GetChunkUnloadRadius() const;
    float GetChunkWidth() const;

public:

    WorldSettings m_worldSettings;

    // lifetime owned by SWorld
    std::map<IntVec2, Chunk*> m_activeChunks;
};

