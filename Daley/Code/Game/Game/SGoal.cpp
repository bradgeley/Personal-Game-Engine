// Bradley Christensen - 2022-2026
#include "SGoal.h"
#include "CTags.h"
#include "CTransform.h"
#include "SCDebug.h"
#include "SCEntityFactory.h"
#include "SCRunData.h"
#include "SCWorld.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
void SGoal::Startup()
{
	AddReadDependencies<CTags, CTransform, SCDebug, SCWorld>();
	AddWriteDependencies<SCEntityFactory, SCRunData>();

	m_runWhilePaused = false;
}



//----------------------------------------------------------------------------------------------------------------------
void SGoal::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto const& tagsStorage = context.GetArrayStorageConst<CTags>();
	auto const& transStorage =context.GetArrayStorageConst<CTransform>();
	auto const& world = context.GetSingletonConst<SCWorld>();
	auto const& scDebug = context.GetSingletonConst<SCDebug>();

	// Write Dependencies
	SCEntityFactory& entityFactory = context.GetSingleton<SCEntityFactory>();
	SCRunData& scRunData = context.GetSingleton<SCRunData>();
	RunData& runData = *scRunData.m_data;

	Name enemyTag = "Enemy";

	for (auto it = context.Iterate<CTransform, CTags>(); it.IsValid(); ++it)
	{
		CTags const& tags = tagsStorage[it];

		if (!tags.HasTag(enemyTag))
		{
			continue;
		}

		CTransform const& transform = transStorage[it];

		if (world.IsLocationInGoal(transform.m_pos))
		{
			entityFactory.m_entitiesToDestroy.push_back(it.GetEntityID());

			if (!scDebug.m_godMode)
			{
				float damage = GetDamageToPlayerByTags(tags);
				runData.m_health -= damage;
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
float SGoal::GetDamageToPlayerByTags(CTags const& tags)
{
	float damage = 0.f;
	if (tags.HasTag("small"))
	{
		damage += 1.f;
	}
	else if (tags.HasTag("medium"))
	{
		damage += 2.f;
	}
	else if (tags.HasTag("large"))
	{
		damage += 3.f;
	}
	else if (tags.HasTag("boss"))
	{
		damage += 25.f;
	}
	if (tags.HasTag("magic"))
	{
		damage *= 2.f;
	}
	else if (tags.HasTag("rare"))
	{
		damage *= 5.f;
	}
	return damage;
}
