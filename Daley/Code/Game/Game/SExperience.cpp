// Bradley Christensen - 2022-2026
#include "SExperience.h"
#include "CDeath.h"
#include "SCRunData.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SExperience::Startup()
{
	AddReadDependencies<CDeath>();
	AddWriteDependencies<SCRunData>();

	DevConsoleUtils::AddDevConsoleCommand("GrantExp", &SExperience::GrantExp, "exp", DevConsoleArgType::UInt);
}



//----------------------------------------------------------------------------------------------------------------------
void SExperience::Shutdown() const
{
	DevConsoleUtils::RemoveDevConsoleCommand("GrantExp", &SExperience::GrantExp);
}



//----------------------------------------------------------------------------------------------------------------------
void GrantExpInternal(RunData& runData, uint64_t exp)
{
	int levelBefore = RunData::GetLevelData(runData.m_experience).m_level;
	runData.m_experience += exp;
	if (runData.m_experience > StaticGameSettings::s_maximumExperience)
	{
		runData.m_experience = StaticGameSettings::s_maximumExperience;
	}
	int levelAfter = RunData::GetLevelData(runData.m_experience).m_level;

	int levelsGained = levelAfter - levelBefore;
	runData.m_numModifierChoicesRemaining += levelsGained;
}



//----------------------------------------------------------------------------------------------------------------------
void SExperience::Run(SystemContext const& context) const
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
			GrantExpInternal(runData, static_cast<uint64_t>(death.m_expReward));
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool SExperience::GrantExp(NamedProperties& params)
{
	SCRunData& scRunData = g_ecs->GetSingleton<SCRunData>();
	RunData& runData = *scRunData.m_data;
	uint32_t exp = params.Get<uint32_t>("exp", 0);

	GrantExpInternal(runData, static_cast<uint64_t>(exp));

	return false;
}
