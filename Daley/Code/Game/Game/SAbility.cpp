// Bradley Christensen - 2022-2026
#include "SAbility.h"
#include "CTransform.h"
#include "CAbility.h"
#include "AbilityDef.h"



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Startup()
{
	AbilityDef::LoadFromXML();

	AddWriteAllDependencies();
}



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Run(SystemContext const& context)
{
	auto const& transStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& abilityStorage = g_ecs->GetMapStorage<CAbility>();

	for (auto it = g_ecs->Iterate<CAbility, CTransform>(context); it.IsValid(); ++it)
	{
		CAbility& ability = *abilityStorage.Get(it);
		CTransform const& transform = *transStorage.Get(it);

		for (Ability* abilityInstance : ability.m_abilities)
		{
			abilityInstance->Update(context.m_deltaSeconds, transform.m_pos);
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Shutdown()
{
	AbilityDef::Shutdown();
}
