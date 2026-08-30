// Bradley Christensen - 2022-2026
#include "SAbility.h"
#include "Ability.h"
#include "AbilityDef.h"
#include "CAbility.h"
#include "CTags.h"
#include "CTime.h"
#include "CTransform.h"
#include "SCRunData.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Startup()
{
	AbilityDef::LoadFromXML();

	AddWriteAllDependencies(); // Abilities spawn things

	m_runWhilePaused = false;
}



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Shutdown() const
{
	AbilityDef::Shutdown();
}



//----------------------------------------------------------------------------------------------------------------------
Ability* RebuildAbility(Ability* ability, CTags const& tags, SystemContext const& context)
{
	SCRunData const& scRunData = context.GetSingletonConst<SCRunData>();
	RunData const& runData = *scRunData.m_data;

	Ability* newAbility = ability->m_abilityDef->MakeAbilityInstance();

	for (RunModifier const* mod : runData.m_activeRunModifiers)
	{
		mod->ApplyToAbility(*newAbility, tags);
	}

	ability->CopyTransientDataTo(*newAbility);

	ability->Shutdown(context);
	delete ability;
	ability = nullptr;

	newAbility->m_needsRebuild = false;
	return newAbility;
}



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto& transStorage = context.GetArrayStorageConst<CTransform>();
	auto& timeStorage = context.GetArrayStorageConst<CTime>();
	auto& tagsStorage = context.GetArrayStorageConst<CTags>();

	// Write Dependencies
	auto& abilityStorage = context.GetMapStorage<CAbility>();
	// Spawning, anything else that abilities use

	for (auto it = context.Iterate<CAbility, CTags, CTime, CTransform>(); it.IsValid(); ++it)
	{
		CAbility& ability = abilityStorage[it];
		CTags const& tags = tagsStorage[it];
		CTime const& time = timeStorage[it];
		CTransform const& transform = transStorage[it];

		float timeDilation = time.m_clock.GetTimeDilationF();

		for (Ability*& abilityInstance : ability.m_abilities)
		{
			if (abilityInstance->m_needsRebuild)
			{
				abilityInstance = RebuildAbility(abilityInstance, tags, context);
			}
		}

		for (Ability* abilityInstance : ability.m_abilities)
		{
			abilityInstance->Update(context, transform.m_pos, timeDilation);
		}
	}
}
