// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/IntVec2.h"
#include "Engine/ECS/EntityID.h"
#include <unordered_map>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct ChunkCollisionData
{
	std::vector<EntityID> m_entitiesTouchingChunk;
	std::vector<std::vector<EntityID>> m_entitiesTouchingTile;
};



//----------------------------------------------------------------------------------------------------------------------
class SCCollision
{
public:

	std::unordered_map<IntVec2, ChunkCollisionData> m_chunkCollisionData;
};

