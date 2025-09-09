// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/AABB2.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/ECS/EntityID.h"
#include <unordered_map>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
typedef std::vector<EntityID> EntityBucket;



//----------------------------------------------------------------------------------------------------------------------
struct ChunkCollisionData
{
	EntityBucket m_chunkBucket;
	std::unordered_map<int, EntityBucket> m_tileBuckets;
};



//----------------------------------------------------------------------------------------------------------------------
class SCCollision
{
public:

	AABB2 m_collisionUpdateBounds = AABB2::ZeroToOne;						// Updated by collision hash system
	std::unordered_map<IntVec2, ChunkCollisionData> m_chunkCollisionData;

	// Used by collision hash system to split work, then is combined into m_chunkCollisionData in Post-Run
	std::vector<std::unordered_map<IntVec2, ChunkCollisionData>> m_perThreadCollisionData;
};

