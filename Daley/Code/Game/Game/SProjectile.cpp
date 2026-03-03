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
		float moveDistThisFrame = proj.m_projSpeedUnitsPerSec * moveTimeThisFrame;
		float distSquaredToTarget = toTarget.GetLengthSquared();
		if (distSquaredToTarget <= moveDistThisFrame * moveDistThisFrame)
		{
			// Projectile hit target
			factory.m_entitiesToDestroy.push_back(it.GetEntityID());

			if (proj.m_onHitComp.has_value())
			{
				float damage = 0.f;
				if (proj.m_onHitComp->m_damageOnHit.has_value())
				{
					damage = g_rng->GetRandomFloatInRange(proj.m_onHitComp->m_damageOnHit->m_minDamage, proj.m_onHitComp->m_damageOnHit->m_maxDamage);
				}

				float burn = 0.f;
				if (proj.m_onHitComp->m_burnOnHit.has_value())
				{
					burn = proj.m_onHitComp->m_burnOnHit->m_burn;
				}

				float poison = 0.f;
				if (proj.m_onHitComp->m_poisonOnHit.has_value())
				{
					poison = proj.m_onHitComp->m_poisonOnHit->m_poison;
				}

				if (damage > 0.f)
				{
					CHealth* targetHealth = healthStorage.Get(proj.m_targetID.GetIndex());
					if (targetHealth)
					{
						targetHealth->TakeDamage(damage);
					}
				}

				if (proj.m_onHitComp->m_aoeHitOnHit.has_value())
				{
					float const& splashRadius = proj.m_onHitComp->m_aoeHitOnHit->m_radius;
					float const& splashRadiusSquared = splashRadius * splashRadius;

					float aoeDamage = 0.f;
					if (proj.m_onHitComp->m_aoeHitOnHit->m_damageOnHit.has_value())
					{
						aoeDamage = g_rng->GetRandomFloatInRange(proj.m_onHitComp->m_aoeHitOnHit->m_damageOnHit->m_minDamage, proj.m_onHitComp->m_aoeHitOnHit->m_damageOnHit->m_maxDamage);
					}

					float aoeBurn = 0.f;
					if (proj.m_onHitComp->m_aoeHitOnHit->m_burnOnHit.has_value())
					{
						aoeBurn = proj.m_onHitComp->m_aoeHitOnHit->m_burnOnHit->m_burn;
					}

					float poison = 0.f;
					if (proj.m_onHitComp->m_aoeHitOnHit->m_poisonOnHit.has_value())
					{
						poison = proj.m_onHitComp->m_aoeHitOnHit->m_poisonOnHit->m_poison;
					}

					world.ForEachPathTileOverlappingCircle(transform.m_pos, splashRadius, [&](IntVec2 const& worldCoords)
					{
						int tileIndex = world.m_tiles.GetIndexForCoords(worldCoords);
						for (auto& entityID : scCollision.m_tileBuckets[tileIndex])
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

							if (aoeDamage > 0.f)
							{
								CHealth* targetHealth = healthStorage.Get(entityID.GetIndex());
								if (targetHealth)
								{
									targetHealth->TakeDamage(aoeDamage);
								}
							}
						}
						return true;
					});
				}
			}

			continue;
		}
		Vec2 moveThisFrame = toTarget.GetNormalized() * proj.m_projSpeedUnitsPerSec * moveTimeThisFrame;
		transform.m_pos += moveThisFrame;
		transform.m_orientation = toTarget.GetAngleDegrees();
	}
}