// Bradley Christensen - 2022-2026
#include "SAbility.h"
#include "Ability.h"
#include "CAbility.h"
#include "CTags.h"
#include "CTime.h"
#include "CTransform.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Startup()
{
	AddWriteAllDependencies(); // Abilities spawn things

	m_runWhilePaused = true;
}



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Shutdown() const
{
}



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto& transStorage = context.GetArrayStorageConst<CTransform>();
	auto& timeStorage = context.GetArrayStorageConst<CTime>();
	//auto& tagsStorage = context.GetArrayStorageConst<CTags>();

	// Write Dependencies
	auto& abilityStorage = context.GetMapStorage<CAbility>();
	// Spawning, anything else that abilities use

	for (auto it = context.Iterate<CAbility, CTags, CTime, CTransform>(); it.IsValid(); ++it)
	{
		CAbility& ability = abilityStorage[it];
		//CTags const& tags = tagsStorage[it];
		CTime const& time = timeStorage[it];
		CTransform const& transform = transStorage[it];

		float timeDilation = time.m_clock.GetTimeDilationF();

		if (ability.m_needsAttributeRebuild)
		{
			//ability.RebuildAttributes(context);
		}

		if (context.m_deltaSeconds <= 0.f)
		{
			continue;
		}

		for (Ability* abilityInstance : ability.m_abilities)
		{
			abilityInstance->Update(context, ability, transform.m_pos, timeDilation);
		}
	}
}
