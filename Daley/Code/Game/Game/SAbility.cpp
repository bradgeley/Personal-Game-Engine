// Bradley Christensen - 2022-2026
#include "SAbility.h"
#include "Ability.h"
#include "CAbility.h"
#include "CHealth.h"
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

	// Write Dependencies
	auto& abilityStorage = context.GetMapStorage<CAbility>();
	auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& timeStorage = context.GetArrayStorage<CTime>();
	// Spawning, anything else that abilities use

	for (auto it = context.Iterate<CHealth>(); it.IsValid(); ++it)
	{
		CHealth& health = healthStorage[it];
		health.SetIsProtectedThisFrame(false);
		health.SetIsSuppressingBurnThisFrame(false);
		health.SetIsSuppressingPoisonThisFrame(false);
	}

	for (auto it = context.Iterate<CTime>(); it.IsValid(); ++it)
	{
		CTime& time = timeStorage[it];
		time.SetIsHastedThisFrame(false);
		time.SetIsSlowedThisFrame(false);
	}

	for (auto it = context.Iterate<CAbility, CTime, CTransform>(); it.IsValid(); ++it)
	{
		CAbility& ability = abilityStorage[it];
		CTime const& time = timeStorage[it];
		CTransform const& transform = transStorage[it];

		if (ability.m_needsAttributeRebuild)
		{
			ability.RebuildAttributes(context, it.GetEntityID());
		}

		if (context.m_deltaSeconds <= 0.f)
		{
			continue;
		}

		float timeDilation = time.m_clock.GetTimeDilationF();

		for (Ability* abilityInstance : ability.m_abilities)
		{
			abilityInstance->Update(context, ability, transform.m_pos, timeDilation);
		}
	}
}
