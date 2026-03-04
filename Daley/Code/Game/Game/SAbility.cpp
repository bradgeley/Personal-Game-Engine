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
	auto& abilityStorage = g_ecs->GetMapStorage<CAbility>();
	auto& transStorage = g_ecs->GetArrayStorage<CTransform>();

	for (auto it = g_ecs->Iterate<CAbility, CTransform>(context); it.IsValid(); ++it)
	{
		CAbility& ability = *abilityStorage.Get(it);
		CTransform const& transform = *transStorage.Get(it);

		for (Ability* w : ability.m_abilities)
		{
			w->Update(context.m_deltaSeconds, transform.m_pos);
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Shutdown()
{
	AbilityDef::Shutdown();
}
