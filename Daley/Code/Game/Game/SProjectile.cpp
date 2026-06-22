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
	AddWriteAllDependencies();
}



//----------------------------------------------------------------------------------------------------------------------
void SProjectile::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCWorld const& world = context.GetSingletonConst<SCWorld>();
	SCCollision const& scCollision = context.GetSingletonConst<SCCollision>();

	// Write Dependencies
	auto& factory = context.GetSingleton<SCEntityFactory>();
	auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& projStorage = context.GetMapStorage<CProjectile>();
	auto& transStorage = context.GetArrayStorage<CTransform>();
	auto& timeStorage = context.GetArrayStorage<CTime>();
	auto& collisionEffectStorage = context.GetArrayStorage<CCollisionEffect>();
	// Spawn Entity (All)

	CollisionLayer const& enemyLayer = scCollision.GetCollisionLayer(CollisionChannel::Enemy);

	for (auto it = context.Iterate<CProjectile, CTransform>(); it.IsValid(); ++it)
	{
		CProjectile& proj = projStorage[it];
		if (proj.m_targetID == EntityID::Invalid || !context.IsValid(proj.m_targetID))
		{
			if (!proj.m_targetPos.has_value())
			{
				factory.m_entitiesToDestroy.push_back(it.GetEntityID());
				continue;
			}
		}

		CTransform& transform = transStorage[it];
		CTransform& targetTransform = transStorage[proj.m_targetID];
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
					if (mainTargetPayload.IsRelevantToHealth())
					{
						if (context.HasComponent<CHealth>(proj.m_targetID))
						{
							CHealth& targetHealth = healthStorage[proj.m_targetID];
							targetHealth.TakePayload(mainTargetPayload);
						}
					}
					if (mainTargetPayload.IsRelevantToTime())
					{
						// Add slow effect to target
						if (context.HasComponent<CTime>(proj.m_targetID))
						{
							CTime& targetTime = timeStorage[proj.m_targetID];
							targetTime.m_remainingSlowDuration += mainTargetPayload.m_slowDuration;
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
								Vec2 entityPos = transStorage[entityID].m_pos;
								if (entityPos.GetDistanceSquaredTo(proj.m_targetPos.value()) > splashRadiusSquared)
								{
									// outside of splash radius
									continue;
								}

								if (aoeTargetPayload.IsRelevantToHealth())
								{
									if (context.HasComponent<CHealth>(entityID))
									{
										CHealth& targetHealth = healthStorage[entityID];
										targetHealth.TakePayload(aoeTargetPayload);
									}
								}

								if (aoeTargetPayload.IsRelevantToTime())
								{
									if (context.HasComponent<CTime>(entityID))
									{
										CTime& targetTime = timeStorage[entityID];
										targetTime.m_remainingSlowDuration += aoeTargetPayload.m_slowDuration;
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

					EntityID aoeEffect = SEntityFactory::SpawnEntity(context, aoeEffectSpawnInfo);
					if (context.IsValid(aoeEffect))
					{
						// Pass along damage, color, to aoe effect
						if (context.HasComponent<CCollisionEffect>(aoeEffect))
						{
							CCollisionEffect& aoeEffectComp = collisionEffectStorage[aoeEffect];
							aoeEffectComp.InitializeFromAoEEffect(proj.m_onHitComp->m_aoeEffectOnHit.value());
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