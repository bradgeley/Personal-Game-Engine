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
    AddWriteDependencies<CTransform, SCCollision>();
    AddReadDependencies<CCollision>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCollision::PreRun()
{
	SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();

	std::swap(scCollision.m_lastFrameOverlaps, scCollision.m_frameOverlaps);
	scCollision.m_frameOverlaps.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Run(SystemContext const&)
{
    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();
	
	auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& collisionStorage = g_ecs->GetArrayStorage<CCollision>();

	for (CollisionLayer& layer : scCollision.m_collisionLayers)
	{
		for (int bucketIndex = 0; bucketIndex < (int) layer.size(); ++bucketIndex)
		{
			CollisionBucket& bucket = layer[bucketIndex];

			for (int entityIndex = 0; entityIndex < (int) bucket.size(); ++entityIndex)
			{
				EntityID entityID = bucket[entityIndex];
				CCollision& collision = *collisionStorage.Get(entityID.GetIndex());

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

							CTransform& transform = *transformStorage.Get(entityID.GetIndex());

							CCollision& otherCollision = *collisionStorage.Get(otherEntityID.GetIndex());
							CTransform& otherTransform = *transformStorage.Get(otherEntityID.GetIndex());

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