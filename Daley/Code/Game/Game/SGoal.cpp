// Bradley Christensen - 2022-2026
#include "SGoal.h"
#include "CHealth.h"
#include "CTransform.h"
#include "SCEntityFactory.h"
#include "SCWorld.h"



//----------------------------------------------------------------------------------------------------------------------
void SGoal::Startup()
{
	AddReadDependencies<CHealth, CTransform, SCWorld>();
	AddWriteDependencies<SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SGoal::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto const& healthStorage = context.GetArrayStorageConst<CHealth>();
	auto const& transStorage =context.GetArrayStorageConst<CTransform>();
	auto const& world = context.GetSingletonConst<SCWorld>();

	// Write Dependencies
	auto& entityFactory = context.GetSingleton<SCEntityFactory>();

	for (auto it = context.Iterate<CTransform, CHealth>(); it.IsValid(); ++it)
	{
		CHealth const& health = healthStorage[it];

		if (!health.GetIsTargetable())
		{
			continue;
		}

		CTransform const& transform = transStorage[it];

		if (world.IsLocationInGoal(transform.m_pos))
		{
			entityFactory.m_entitiesToDestroy.push_back(it.GetEntityID());
		}
	}
}
