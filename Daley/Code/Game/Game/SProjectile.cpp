// Bradley Christensen - 2022-2026
#include "SProjectile.h"
#include "CProjectile.h"
#include "CPuddle.h"
#include "CTransform.h"
#include "CCollision.h"
#include "CHealth.h"
#include "CTime.h"
#include "EntityDef.h"
#include "SCCollision.h"
#include "SCEntityFactory.h"
#include "SCWorld.h"
#include "SEntityFactory.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
void SProjectile::Startup()
{
	AddWriteAllDependencies();

	m_runWhilePaused = false;
}



//----------------------------------------------------------------------------------------------------------------------
EntityID GetNextValidProjTarget(CProjectile& proj, SystemContext const& context)
{
	while (proj.NextTarget())
	{
		EntityID nextTarget = proj.GetTarget();
		if (nextTarget != EntityID::Invalid && context.IsValid(nextTarget))
		{
			return nextTarget;
		}
	}
	return EntityID::Invalid;
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
	auto& puddleStorage = context.GetArrayStorage<CPuddle>();
	// Spawn Entity (All)

	BitMask puddleBit = context.GetComponentBitMask<CPuddle>();

	for (auto it = context.Iterate<CProjectile, CTransform>(); it.IsValid(); ++it)
	{
		CProjectile& proj = projStorage[it];
		EntityID target = proj.GetTarget();
		
		if (target == EntityID::Invalid || !context.IsValid(target))
		{
			if (!proj.m_targetPos.has_value())
			{
				factory.m_entitiesToDestroy.push_back(it.GetEntityID());
				continue;
			}
		}
		else
		{
			CTransform& targetTransform = transStorage[target];
			proj.m_targetPos = targetTransform.m_pos;
		}

		CTransform& transform = transStorage[it];
		float moveTimeThisFrame = context.m_deltaSeconds + proj.m_accumulatedTime;
		float moveDistThisFrame = proj.m_projSpeed * moveTimeThisFrame;
		Vec2 toTarget = proj.m_targetPos.value() - transform.m_pos;
		Vec2 moveThisFrame = toTarget.GetNormalized() * proj.m_projSpeed * moveTimeThisFrame;
		proj.m_accumulatedTime = 0.f;

		// Perform proj movement
		transform.m_pos += moveThisFrame;
		transform.m_orientation = toTarget.GetAngleDegrees();

		// Check if near destination
		float distSquaredToTarget = toTarget.GetLengthSquared();
		if (distSquaredToTarget <= moveDistThisFrame * moveDistThisFrame)
		{
			transform.m_pos = proj.m_targetPos.value();

			// Projectile hit target
			EntityID nextTarget = GetNextValidProjTarget(proj, context);
			if (nextTarget == EntityID::Invalid)
			{
				factory.m_entitiesToDestroy.push_back(it.GetEntityID());
			}

			if (proj.m_onHitComp.IsRelevant())
			{
				HitPayload mainTargetPayload = proj.GetMainTargetPayload();
				if (mainTargetPayload.HasValue())
				{
					if (mainTargetPayload.IsRelevantToHealth())
					{
						CHealth& targetHealth = healthStorage[target];
						targetHealth.TakePayload(mainTargetPayload);
					}
					if (mainTargetPayload.IsRelevantToTime())
					{
						CTime& targetTime = timeStorage[target];
						targetTime.TakePayload(mainTargetPayload);
					}
				}

				if (proj.m_onHitComp.m_splashOnHit.IsRelevant())
				{
					// Projectile is doing aoe damage around target
					float const& splashRadius = proj.m_onHitComp.m_splashOnHit.m_radius;
					float const& splashRadiusSquared = splashRadius * splashRadius;

					HitPayload splashPayload = proj.GetSplashPayload();
					if (splashPayload.HasValue())
					{
						world.ForEachPathTileOverlappingCircle(transform.m_pos, splashRadius, [&](IntVec2 const& worldCoords)
						{
							int tileIndex = world.m_tiles.GetIndexForCoords(worldCoords);

							scCollision.ForEachEnemyInTile(tileIndex, [&](EntityID entityID)
							{
                                if (!context.IsValid(entityID))
                                {
                                    return true;
                                }

								Vec2 entityPos = transStorage[entityID].m_pos;
								if (entityPos.GetDistanceSquaredTo(proj.m_targetPos.value()) > splashRadiusSquared)
								{
									// outside of splash radius
									return true;
								}

								if (splashPayload.IsRelevantToHealth())
								{
									CHealth& targetHealth = healthStorage[entityID];
									targetHealth.TakePayload(splashPayload);
								}

								if (splashPayload.IsRelevantToTime())
								{
									CTime& targetTime = timeStorage[entityID];
									targetTime.TakePayload(splashPayload);
								}
								return true;
							});

							return true;
						});
					}
				}

				if (proj.m_onHitComp.m_puddleOnHit.IsRelevant())
				{
					SpawnInfo puddleSpawnInfo;
					puddleSpawnInfo.m_def = EntityDef::GetEntityDef(proj.m_onHitComp.m_puddleOnHit.m_puddleDefName);
					ASSERT_OR_DIE(puddleSpawnInfo.m_def != nullptr, StringUtils::StringF("EntityDef not found for name: %s", proj.m_onHitComp.m_puddleOnHit.m_puddleDefName.ToCStr()));
					puddleSpawnInfo.m_spawnPos = proj.m_targetPos.value();
					puddleSpawnInfo.m_spawnOrientation = 0.f;
					puddleSpawnInfo.m_spawnLifetime = proj.m_onHitComp.m_puddleOnHit.m_durationSeconds;
					puddleSpawnInfo.m_spawnScale = proj.m_onHitComp.m_puddleOnHit.m_radius;

					EntityID puddle = SEntityFactory::SpawnEntity(context, puddleSpawnInfo);
					if (context.IsValid(puddle))
					{
						// Pass along damage, color, to puddle effect
						ASSERT_OR_DIE(context.HasComponentsUnsafe(puddle.GetIndex(), puddleBit), "Spawned puddle does not have CPuddle component.");
						CPuddle& puddleComp = puddleStorage[puddle];
						puddleComp.Initialize(proj.m_onHitComp.m_puddleOnHit);
					}
				}
			}
		}
	}
}