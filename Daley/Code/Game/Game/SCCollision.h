// Bradley Christensen - 2022-2026
#pragma once
#include "CollisionProfile.h"
#include "Engine/ECS/EntityID.h"
#include "OverlapInfo.h"
#include <vector>
#include <unordered_set>



//----------------------------------------------------------------------------------------------------------------------
typedef std::vector<EntityID> CollisionBucket;
typedef std::vector<CollisionBucket> CollisionLayer;



//----------------------------------------------------------------------------------------------------------------------
class SCCollision
{
public:

	CollisionLayer& GetCollisionLayer(CollisionChannel channel);
	CollisionLayer const& GetCollisionLayer(CollisionChannel channel) const;

public:

	std::vector<CollisionLayer> m_collisionLayers;
	std::unordered_set<int> m_dirtyBuckets;

	std::unordered_set<OverlapInfo> m_lastFrameOverlaps;
	std::unordered_set<OverlapInfo> m_frameOverlaps;
};

