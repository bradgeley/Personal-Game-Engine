// Bradley Christensen - 2022-2026
#include "SGoal.h"
#include "CHealth.h"
#include "CTransform.h"
#include "SCEntityFactory.h"
#include "SCWorld.h"



//----------------------------------------------------------------------------------------------------------------------
void SGoal::Startup()
{
	AddWriteDependencies<SCEntityFactory>();
	AddReadDependencies<CTransform, CHealth, SCWorld>();
}



//----------------------------------------------------------------------------------------------------------------------
void SGoal::Run(SystemContext const& context)
{
	auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& healthStorage = g_ecs->GetArrayStorage<CHealth>();
	auto& world = g_ecs->GetSingleton<SCWorld>();
	auto& entityFactory = g_ecs->GetSingleton<SCEntityFactory>();

	for (auto it = g_ecs->Iterate<CTransform, CHealth>(context); it.IsValid(); ++it)
	{
		CHealth const& health = *healthStorage.Get(it);

		if (!health.GetIsTargetable())
		{
			continue;
		}

		CTransform const& transform = *transStorage.Get(it);

		if (world.IsLocationInGoal(transform.m_pos))
		{
			entityFactory.m_entitiesToDestroy.push_back(it.GetEntityID());
		}
	}
}
