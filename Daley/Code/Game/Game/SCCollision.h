// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/EntityID.h"
#include "OverlapInfo.h"
#include <vector>
#include <unordered_set>



//----------------------------------------------------------------------------------------------------------------------
typedef std::vector<EntityID> EntityBucket;



//----------------------------------------------------------------------------------------------------------------------
class SCCollision
{
public:

	// Todo: add layers (Enemy layer)
	std::vector<EntityBucket> m_tileBuckets;
	std::unordered_set<OverlapInfo> m_lastFrameOverlaps;
	std::unordered_set<OverlapInfo> m_frameOverlaps;
};

