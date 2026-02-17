// Bradley Christensen - 2022-2025
#include "SProjectile.h"
#include "CProjectile.h"
#include "CTransform.h"
#include "SCEntityFactory.h"



//----------------------------------------------------------------------------------------------------------------------
void SProjectile::Startup()
{
	AddWriteDependencies<CProjectile, CTransform, SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SProjectile::Run(SystemContext const& context)
{
	auto& projStorage = g_ecs->GetMapStorage<CProjectile>();
	auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& factory = g_ecs->GetSingleton<SCEntityFactory>();

	for (auto it = g_ecs->Iterate<CProjectile, CTransform>(context); it.IsValid(); ++it)
	{
		CProjectile& proj = *projStorage.Get(it);
		if (proj.m_targetID == ENTITY_ID_INVALID)
		{
			factory.m_entitiesToDestroy.push_back(it.m_currentIndex);
			continue;
		}

		CTransform& transform = *transStorage.Get(it);
		CTransform& targetTransform = *transStorage.Get(proj.m_targetID);

		Vec2 toTarget = targetTransform.m_pos - transform.m_pos;
		float moveTimeThisFrame = context.m_deltaSeconds + proj.m_accumulatedTime;
		proj.m_accumulatedTime = 0.f;
		float moveDistThisFrame = proj.m_projSpeedUnitsPerSec * moveTimeThisFrame;
		float distSquaredToTarget = toTarget.GetLengthSquared();
		if (distSquaredToTarget <= moveDistThisFrame * moveDistThisFrame)
		{
			// Hit target, destroy proj
			factory.m_entitiesToDestroy.push_back(it.m_currentIndex);
			factory.m_entitiesToDestroy.push_back(proj.m_targetID);
			continue;
		}
		Vec2 moveThisFrame = toTarget.GetNormalized() * proj.m_projSpeedUnitsPerSec * moveTimeThisFrame;
		transform.m_pos += moveThisFrame;
		transform.m_orientation = toTarget.GetAngleDegrees();
	}
}