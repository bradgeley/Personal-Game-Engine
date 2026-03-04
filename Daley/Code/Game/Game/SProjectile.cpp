// Bradley Christensen - 2022-2026
#include "SProjectile.h"
#include "CProjectile.h"
#include "CTransform.h"
#include "CCollision.h"
#include "CHealth.h"
#include "SCCollision.h"
#include "SCEntityFactory.h"
#include "SCWorld.h"
#include "Engine/Math/RandomNumberGenerator.h"



//----------------------------------------------------------------------------------------------------------------------
void SProjectile::Startup()
{
	AddWriteDependencies<CProjectile, CTransform, CHealth, SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SProjectile::Run(SystemContext const& context)
{
	auto& projStorage = g_ecs->GetMapStorage<CProjectile>();
	auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& collisionStorage = g_ecs->GetArrayStorage<CCollision>();
	auto& healthStorage = g_ecs->GetArrayStorage<CHealth>();
	auto& factory = g_ecs->GetSingleton<SCEntityFactory>();
	auto& world = g_ecs->GetSingleton<SCWorld>();
	auto& scCollision = g_ecs->GetSingleton<SCCollision>();

	for (auto it = g_ecs->Iterate<CProjectile, CTransform>(context); it.IsValid(); ++it)
	{
		CProjectile& proj = *projStorage.Get(it);
		if (proj.m_targetID == EntityID::Invalid || !g_ecs->IsValid(proj.m_targetID))
		{
			if (!proj.m_targetPos.has_value())
			{
				factory.m_entitiesToDestroy.push_back(it.GetEntityID());
				continue;
			}
		}

		CTransform& transform = *transStorage.Get(it);
		CTransform& targetTransform = *transStorage.Get(proj.m_targetID.GetIndex());
		proj.m_targetPos = targetTransform.m_pos;

		Vec2 toTarget = proj.m_targetPos.value() - transform.m_pos;
		float moveTimeThisFrame = context.m_deltaSeconds + proj.m_accumulatedTime;
		proj.m_accumulatedTime = 0.f;
		float moveDistThisFrame = proj.m_projSpeed * moveTimeThisFrame;
		float distSquaredToTarget = toTarget.GetLengthSquared();
		if (distSquaredToTarget <= moveDistThisFrame * moveDistThisFrame)
		{
			// Projectile hit target
			factory.m_entitiesToDestroy.push_back(it.GetEntityID());

			proj.RollDamageAndEffects();

			if (proj.m_onHitComp.has_value())
			{
				HitPayload mainTargetPayload = proj.GetMainTargetPayload();
				if (mainTargetPayload.HasValue())
				{
					CHealth* targetHealth = healthStorage.Get(proj.m_targetID.GetIndex());
					if (targetHealth)
					{
						targetHealth->TakePayload(mainTargetPayload);
					}
				}

				if (proj.m_onHitComp->m_aoeHitOnHit.has_value())
				{
					// Projectile is doing aoe damage around target
					float const& splashRadius = proj.m_onHitComp->m_aoeHitOnHit->m_radius;
					float const& splashRadiusSquared = splashRadius * splashRadius;

					HitPayload aoeTargetPayload = proj.GetAoeTargetPayload();
					if (aoeTargetPayload.HasValue())
					{
						world.ForEachPathTileOverlappingCircle(transform.m_pos, splashRadius, [&](IntVec2 const& worldCoords)
						{
							int tileIndex = world.m_tiles.GetIndexForCoords(worldCoords);
							for (EntityID const& entityID : scCollision.m_tileBuckets[tileIndex])
							{
								if (entityID == proj.m_targetID)
								{
									continue; // already damaged by direct hit
								}
								CCollision* collision = collisionStorage.Get(entityID.GetIndex());
								if (collision)
								{
									Vec2 entityPos = transStorage.Get(entityID.GetIndex())->m_pos;
									if (entityPos.GetDistanceSquaredTo(proj.m_targetPos.value()) > splashRadiusSquared)
									{
										continue; // outside of splash radius
									}
								}

								CHealth* targetHealth = healthStorage.Get(entityID.GetIndex());
								if (targetHealth)
								{
									targetHealth->TakePayload(aoeTargetPayload);
								}
							}
							return true;
						});
					}
				}
			}

			continue;
		}
		Vec2 moveThisFrame = toTarget.GetNormalized() * proj.m_projSpeed * moveTimeThisFrame;
		transform.m_pos += moveThisFrame;
		transform.m_orientation = toTarget.GetAngleDegrees();
	}
}