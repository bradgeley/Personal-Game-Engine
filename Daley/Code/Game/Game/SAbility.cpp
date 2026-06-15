// Bradley Christensen - 2022-2026
#include "SAbility.h"
#include "CTransform.h"
#include "CAbility.h"
#include "AbilityDef.h"



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Startup()
{
	AbilityDef::LoadFromXML();

	AddWriteAllDependencies(); // Abilities spawn things
}



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Shutdown() const
{
	AbilityDef::Shutdown();
}



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto& transStorage = context.GetArrayStorageConst<CTransform>();

	// Write Dependencies
	auto& abilityStorage = context.GetMapStorage<CAbility>();

	for (auto it = context.Iterate<CAbility, CTransform>(); it.IsValid(); ++it)
	{
		CAbility& ability = abilityStorage[it];
		CTransform const& transform = transStorage[it];

		for (Ability* abilityInstance : ability.m_abilities)
		{
			abilityInstance->Update(context, transform.m_pos);
		}
	}
}
