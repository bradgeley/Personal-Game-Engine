// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/AABB2.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/ECS/EntityID.h"
#include <unordered_map>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
typedef std::vector<EntityID> EntityBucket;



//----------------------------------------------------------------------------------------------------------------------
class SCCollision
{
public:

	// Todo: add layers (Enemy layer)
	std::vector<EntityBucket> m_tileBuckets;
};

