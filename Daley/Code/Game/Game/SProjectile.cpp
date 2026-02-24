// Bradley Christensen - 2022-2026
#include "SProjectile.h"
#include "CProjectile.h"
#include "CTransform.h"
#include "CHealth.h"
#include "SCCollision.h"
#include "SCEntityFactory.h"
#include "SCWorld.h"



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

			CHealth* targetHealth = healthStorage.Get(proj.m_targetID.GetIndex());
			if (targetHealth)
			{
				targetHealth->TakeDamage(proj.m_damage);
			}

			if (proj.m_splashComp.has_value())
			{
				world.ForEachPathTileOverlappingCircle(transform.m_pos, proj.m_splashComp->m_splashRadius, [&](IntVec2 const& worldCoords)
				{
					int tileIndex = world.m_tiles.GetIndexForCoords(worldCoords);
					for (auto& entityID : scCollision.m_tileBuckets[tileIndex])
					{
						if (entityID == proj.m_targetID)
						{
							continue; // already damaged by direct hit
						}
						CHealth* health = healthStorage.Get(entityID.GetIndex());
						if (health)
						{
							health->TakeDamage(proj.m_splashComp->m_splashDamage);
						}
					}
					return true;
				});
			}

			continue;
		}
		Vec2 moveThisFrame = toTarget.GetNormalized() * proj.m_projSpeedUnitsPerSec * moveTimeThisFrame;
		transform.m_pos += moveThisFrame;
		transform.m_orientation = toTarget.GetAngleDegrees();
	}
}