// Bradley Christensen - 2022-2026
#include "SRenderAbilities.h"
#include "CTransform.h"
#include "CAbility.h"
#include "AbilityDef.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderAbilities::Startup()
{
	AddWriteAllDependencies();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderAbilities::Shutdown() const
{

}



//----------------------------------------------------------------------------------------------------------------------
void SRenderAbilities::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto& transStorage = context.GetArrayStorageConst<CTransform>();

	// Write Dependencies
	auto& abilityStorage = context.GetMapStorage<CAbility>();
	// Spawning, anything else that abilities use

	for (auto it = context.Iterate<CAbility, CTransform>(); it.IsValid(); ++it)
	{
		CAbility& ability = abilityStorage[it];
		CTransform const& transform = transStorage[it];

		for (Ability* abilityInstance : ability.m_abilities)
		{
			abilityInstance->Render(context, transform.m_pos);
		}
	}
}
