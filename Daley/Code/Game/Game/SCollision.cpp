// Bradley Christensen - 2022-2026
#include "SCollision.h"
#include "CCollision.h"
#include "SCCollision.h"
#include "SCWorld.h"
#include "CTransform.h"
#include "Engine/Math/GeometryUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Startup()
{
    AddReadDependencies<CCollision, CTransform>();
    AddWriteDependencies<SCCollision>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto& transformStorage = context.GetArrayStorageConst<CTransform>();
	auto& collisionStorage = context.GetArrayStorageConst<CCollision>();

	// Write Dependencies
    SCCollision& scCollision = context.GetSingleton<SCCollision>();

	// Swap the previous frame's overlaps into the last frame overlap slot, then clear out this frame for writing to.
	std::swap(scCollision.m_lastFrameOverlaps, scCollision.m_frameOverlaps);
	scCollision.m_frameOverlaps.clear();

	for (int bucketIndex : scCollision.m_dirtyBuckets)
	{
		for (CollisionLayer& layer : scCollision.m_collisionLayers)
		{
			CollisionBucket& bucket = layer[bucketIndex];

			for (int entityIndex = 0; entityIndex < (int) bucket.size(); ++entityIndex)
			{
				EntityID entityID = bucket[entityIndex];
				CCollision const& collision = *collisionStorage.Get(entityID.GetIndex());

				for (int responseChannelIndex = 1; responseChannelIndex < (int) CollisionChannel::Count; ++responseChannelIndex)
				{
					CollisionChannel responseChannel = static_cast<CollisionChannel>(responseChannelIndex);
					if (collision.m_collisionProfile.GetCollisionResponse(responseChannel))
					{
						CollisionBucket& otherBucket = scCollision.m_collisionLayers[responseChannelIndex][bucketIndex];
						for (EntityID otherEntityID : otherBucket)
						{
							if (otherEntityID == entityID)
							{
								continue;
							}

							OverlapInfo overlap(entityID, otherEntityID);
							if (scCollision.m_frameOverlaps.find(overlap) != scCollision.m_frameOverlaps.end())
							{
								continue;
							}

							CTransform const& transform = *transformStorage.Get(entityID.GetIndex());

							CCollision const& otherCollision = *collisionStorage.Get(otherEntityID.GetIndex());
							CTransform const& otherTransform = *transformStorage.Get(otherEntityID.GetIndex());

							Vec2 collisionPos = transform.m_pos + collision.m_offset;
							Vec2 otherCollisionPos = otherTransform.m_pos + otherCollision.m_offset;

							if (GeometryUtils::DoDiscsOverlap2D(collisionPos, collision.m_radius, otherCollisionPos, otherCollision.m_radius))
							{
								// Overlap detected
								scCollision.m_frameOverlaps.emplace(overlap);
							}
						}
					}
				}
			}
		}
	}
}