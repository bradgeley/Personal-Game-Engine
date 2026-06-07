// Bradley Christensen - 2022-2026
#include "SProjectile.h"
#include "CCollisionEffect.h"
#include "CProjectile.h"
#include "CTransform.h"
#include "CCollision.h"
#include "CHealth.h"
#include "CTime.h"
#include "EntityDef.h"
#include "SCCollision.h"
#include "SCEntityFactory.h"
#include "SCWorld.h"
#include "SEntityFactory.h"
#include "Engine/Math/RandomNumberGenerator.h"



//----------------------------------------------------------------------------------------------------------------------
void SProjectile::Startup()
{
	AddReadDependencies<SCWorld, SCCollision>();
	AddWriteDependencies<CProjectile, CTransform, CCollision, CHealth, SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SProjectile::Run(SystemContext const& context)
{
	// Read Dependencies
	auto const& world = g_ecs->GetSingleton<SCWorld>();
	auto const& scCollision = g_ecs->GetSingleton<SCCollision>();

	// Write Dependencies
	auto& projStorage = g_ecs->GetMapStorage<CProjectile>();
	auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& collisionStorage = g_ecs->GetArrayStorage<CCollision>();
	auto& healthStorage = g_ecs->GetArrayStorage<CHealth>();
	auto& factory = g_ecs->GetSingleton<SCEntityFactory>();

	CollisionLayer const& enemyLayer = scCollision.GetCollisionLayer(CollisionChannel::Enemy);

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
					if (mainTargetPayload.m_slowDuration > 0.f)
					{
						// Add slow effect to target
						CTime* time = g_ecs->GetComponent<CTime>(proj.m_targetID);
						if (time)
						{
							time->m_remainingSlowDuration += mainTargetPayload.m_slowDuration;
						}
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
							CollisionBucket const& enemyBucket = enemyLayer[tileIndex];

							for (EntityID const& entityID : enemyBucket)
							{
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

								if (aoeTargetPayload.m_slowDuration > 0.f)
								{
									// Add slow effect to target
									CTime* targetTime = g_ecs->GetComponent<CTime>(entityID);
									if (targetTime)
									{
										targetTime->m_remainingSlowDuration += aoeTargetPayload.m_slowDuration;
									}
								}
							}
							return true;
						});
					}
				}

				if (proj.m_onHitComp->m_aoeEffectOnHit.has_value())
				{
					SpawnInfo aoeEffectSpawnInfo;
					aoeEffectSpawnInfo.m_def = EntityDef::GetEntityDef(proj.m_onHitComp->m_aoeEffectOnHit->m_aoeEffectDefName);
					aoeEffectSpawnInfo.m_spawnPos = proj.m_targetPos.value();
					aoeEffectSpawnInfo.m_spawnOrientation = 0.f;
					aoeEffectSpawnInfo.m_spawnLifetime = proj.m_onHitComp->m_aoeEffectOnHit->m_durationSeconds;
					aoeEffectSpawnInfo.m_spawnScale = proj.m_onHitComp->m_aoeEffectOnHit->m_radius;

					EntityID aoeEffect = SEntityFactory::SpawnEntity(aoeEffectSpawnInfo);
					if (g_ecs->IsValid(aoeEffect))
					{
						// Pass along damage, color, to aoe effect
						if (CCollisionEffect* aoeEffectComp = g_ecs->GetComponent<CCollisionEffect>(aoeEffect))
						{
							if (proj.m_onHitComp->m_aoeEffectOnHit->m_damagePerSecond.has_value())
							{
								aoeEffectComp->m_damagePerSecond = proj.m_onHitComp->m_aoeEffectOnHit->m_damagePerSecond->m_maxDamage;
							}
							if (proj.m_onHitComp->m_aoeEffectOnHit->m_burnPerSecond.has_value())
							{
								aoeEffectComp->m_burnPerSecond = proj.m_onHitComp->m_aoeEffectOnHit->m_burnPerSecond->m_burn;
							}
							if (proj.m_onHitComp->m_aoeEffectOnHit->m_poisonPerSecond.has_value())
							{
								aoeEffectComp->m_poisonPerSecond = proj.m_onHitComp->m_aoeEffectOnHit->m_poisonPerSecond->m_poison;
							}
							if (proj.m_onHitComp->m_aoeEffectOnHit->m_slowPerSecond.has_value())
							{
								aoeEffectComp->m_slowPerSecond = proj.m_onHitComp->m_aoeEffectOnHit->m_slowPerSecond->m_duration;
							}
						}
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