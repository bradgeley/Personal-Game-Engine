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
class SCCollision
{
public:

	std::vector<EntityBucket> m_tileBuckets;
};

