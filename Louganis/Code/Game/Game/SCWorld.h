// Bradley Christensen - 2024
#pragma once
#include "WorldCoords.h"
#include "WorldSettings.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/IntVec2.h"
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
    Chunk* GetActiveChunk(WorldCoords const& worldCoords) const;
    Chunk* GetActiveChunkAtLocation(Vec2 const& worldLocation) const;
    IntVec2 GetChunkCoordsAtLocation(Vec2 const& worldLocation) const;
    IntVec2 GetGlobalTileCoordsAtLocation(Vec2 const& worldLocation) const;
    IntVec2 GetLocalTileCoordsAtLocation(Vec2 const& worldLocation) const;
    IntVec2 GetLocalTileCoordsAtLocation(Vec2 const& worldLocation, IntVec2 const& chunkCoords) const;
    WorldCoords GetWorldCoordsAtOffset(WorldCoords const& worldCoords, IntVec2 const& tileOffset) const;
    WorldCoords GetWorldCoordsAtLocation(Vec2 const& worldLocation) const;
    AABB2 CalculateChunkBounds(int chunkX, int chunkY) const;
    AABB2 GetTileBoundsAtWorldPos(Vec2 const& worldPos) const;
    AABB2 GetTileBounds(WorldCoords const& worldCoords) const;
    AABB2 GetTileBounds(IntVec2 const& worldTileCoords) const;
    
    Chunk* GetOrCreateActiveChunk(int chunkX, int chunkY);
    void RemoveActiveChunk(IntVec2 const& coords);
    void RemoveActiveChunk(int chunkX, int chunkY);
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

