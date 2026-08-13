// Bradley Christensen - 2022-2026
#include "SCurrency.h"
#include "CDeath.h"
#include "SCRunData.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
void SCurrency::Startup()
{
	AddReadDependencies<CDeath>();
	AddWriteDependencies<SCRunData>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCurrency::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto const& deathComponents = context.GetArrayStorageConst<CDeath>();

	// Write Dependencies
	auto& runData = context.GetSingleton<SCRunData>();

	// Go through all entities who died this frame and add their gold to player gold
	for (GroupIter it = context.Iterate<CDeath>(); it.IsValid(); ++it)
	{
		CDeath const& death = deathComponents[it];
		if (death.GetDiedThisFrame())
		{
			runData.m_gold += death.m_goldReward;
		}
	}

	// Tick player health regen (maybe move to separate system later?)
	runData.m_health += runData.m_healthRegen * context.m_deltaSeconds;
	if (runData.m_health > runData.m_maxHealth)
	{
		runData.m_health = runData.m_maxHealth;
	}

	// Tick interest timer
	runData.m_interestTimerSecondsRemaining -= context.m_deltaSeconds;
	
	// Add interest/debt to player gold if interest timer expired
	while (runData.m_interestTimerSecondsRemaining <= 0.f)
	{
		if (runData.m_gold > 0.f)
		{
			runData.m_gold += runData.m_gold * runData.m_savingsInterestRate;
		}
		else if (runData.m_gold < 0.f)
		{
			runData.m_gold += runData.m_gold * runData.m_debtInterestRate;
		}
		runData.m_interestTimerSecondsRemaining += runData.m_interestTimerSeconds;
	}
}
