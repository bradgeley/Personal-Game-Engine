// Bradley Christensen - 2022-2026
#include "SGoal.h"
#include "CTags.h"
#include "CTransform.h"
#include "SCEntityFactory.h"
#include "SCWorld.h"



//----------------------------------------------------------------------------------------------------------------------
void SGoal::Startup()
{
	AddReadDependencies<CTags, CTransform, SCWorld>();
	AddWriteDependencies<SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SGoal::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto const& tagsStorage = context.GetArrayStorageConst<CTags>();
	auto const& transStorage =context.GetArrayStorageConst<CTransform>();
	auto const& world = context.GetSingletonConst<SCWorld>();

	// Write Dependencies
	auto& entityFactory = context.GetSingleton<SCEntityFactory>();

	Name enemyTag = "Enemy";

	for (auto it = context.Iterate<CTransform, CTags>(); it.IsValid(); ++it)
	{
		CTags const& tags = tagsStorage[it];

		if (!tags.FindTag(enemyTag))
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
