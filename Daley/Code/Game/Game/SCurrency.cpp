// Bradley Christensen - 2022-2026
#include "SCurrency.h"
#include "CDeath.h"
#include "SCRunData.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SCurrency::Startup()
{
	AddReadDependencies<CDeath>();
	AddWriteDependencies<SCRunData>();

	DevConsoleUtils::AddDevConsoleCommand("GrantGold", &SCurrency::GrantGold, "gold", DevConsoleArgType::Float);
	DevConsoleUtils::AddDevConsoleCommand("GrantSells", &SCurrency::GrantSells, "sells", DevConsoleArgType::Int);
}



//----------------------------------------------------------------------------------------------------------------------
void SCurrency::Shutdown() const
{
	DevConsoleUtils::RemoveDevConsoleCommand("GrantGold", &SCurrency::GrantGold);
	DevConsoleUtils::RemoveDevConsoleCommand("GrantSells", &SCurrency::GrantSells);
}



//----------------------------------------------------------------------------------------------------------------------
void SCurrency::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto const& deathComponents = context.GetArrayStorageConst<CDeath>();

	// Write Dependencies
	auto& scRunData = context.GetSingleton<SCRunData>();
	RunData& runData = *scRunData.m_data;

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



//----------------------------------------------------------------------------------------------------------------------
bool SCurrency::GrantGold(NamedProperties& params)
{
	SCRunData& scRunData = g_ecs->GetSingleton<SCRunData>();
	RunData& runData = *scRunData.m_data;
	float gold = params.Get<float>("gold", 0.f);
	runData.m_gold += gold;
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SCurrency::GrantSells(NamedProperties& params)
{
	SCRunData& scRunData = g_ecs->GetSingleton<SCRunData>();
	RunData& runData = *scRunData.m_data;
	int sells = params.Get<int>("sells", 0);
	runData.m_numSoldTowers -= sells;
	return false;
}
