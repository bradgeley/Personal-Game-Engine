// Bradley Christensen - 2024
#pragma once
#include "Engine/Math/IntVec2.h"
#include <map>
#include <queue>



class FlowFieldChunk;



//----------------------------------------------------------------------------------------------------------------------
// Data structure for storing info about a tile for the open list, in order of distance
struct ChunkAndTile
{
	IntVec2 m_chunkCoords;
	IntVec2 m_localTileCoords;
	float m_distance = 0.f;

	bool operator<(ChunkAndTile const& rhs) const { return m_distance > rhs.m_distance; }
};



//----------------------------------------------------------------------------------------------------------------------
class SCFlowField
{
public:

	FlowFieldChunk* GetActiveChunk(IntVec2 const& chunkCoords) const;
	void Reset();

public:

	IntVec2 m_lastKnownPlayerLocation = IntVec2(INT_MAX, INT_MAX);
	std::priority_queue<ChunkAndTile> m_openList;
	std::map<IntVec2, FlowFieldChunk*> m_activeFlowFieldChunks;
};

