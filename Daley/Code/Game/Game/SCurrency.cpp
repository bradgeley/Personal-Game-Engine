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
			runData.AddGold(death.m_goldReward);
		}
	}

	// Tick player health regen (maybe move to separate system later?)
	runData.ApplyPlayerHealthRegen(context.m_deltaSeconds);

	// Tick interest timer
	runData.m_incomeIntervalSecondsRemaining -= context.m_deltaSeconds;

	if (runData.m_incomeIntervalSecondsRemaining > 0.f)
	{
		return;
	}

	runData.ApplyPlayerIncome();
}



//----------------------------------------------------------------------------------------------------------------------
bool SCurrency::GrantGold(NamedProperties& params)
{
	SCRunData& scRunData = g_ecs->GetSingleton<SCRunData>();
	RunData& runData = *scRunData.m_data;
	float gold = params.Get<float>("gold", 0.f);
	runData.AddGold(gold);
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SCurrency::GrantSells(NamedProperties& params)
{
	SCRunData& scRunData = g_ecs->GetSingleton<SCRunData>();
	RunData& runData = *scRunData.m_data;
	int sells = params.Get<int>("sells", 0);
	runData.AddSells(sells);
	return false;
}
