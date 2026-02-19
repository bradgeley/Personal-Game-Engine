// Bradley Christensen - 2022-2025
#include "SProjectile.h"
#include "CProjectile.h"
#include "CTransform.h"
#include "CHealth.h"
#include "SCEntityFactory.h"



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

	for (auto it = g_ecs->Iterate<CProjectile, CTransform>(context); it.IsValid(); ++it)
	{
		CProjectile& proj = *projStorage.Get(it);
		if (proj.m_targetID == EntityID::Invalid || !g_ecs->IsValid(proj.m_targetID))
		{
			factory.m_entitiesToDestroy.push_back(it.GetEntityID());
			continue;
		}

		CTransform& transform = *transStorage.Get(it);
		CTransform& targetTransform = *transStorage.Get(proj.m_targetID.GetIndex());

		Vec2 toTarget = targetTransform.m_pos - transform.m_pos;
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

			continue;
		}
		Vec2 moveThisFrame = toTarget.GetNormalized() * proj.m_projSpeedUnitsPerSec * moveTimeThisFrame;
		transform.m_pos += moveThisFrame;
		transform.m_orientation = toTarget.GetAngleDegrees();
	}
}