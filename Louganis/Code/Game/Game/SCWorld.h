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
    AABB2 CalculateChunkBounds(int x, int y) const;
    
    Chunk* GetOrCreateActiveChunk(int x, int y);
    void RemoveActiveChunk(IntVec2 const& coords);
    void RemoveActiveChunk(int x, int y);
    void RemoveActiveChunks(std::vector<IntVec2> const& coordsList);
    void ClearActiveChunks();

    int GetNumTilesInRow() const;
    float GetChunkUnloadRadius() const;

public:

    WorldSettings m_worldSettings;

    // lifetime owned by SWorld
    std::map<IntVec2, Chunk*> m_activeChunks;
};

