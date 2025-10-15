// Bradley Christensen - 2022-2025
#include "SHealth.h"
#include "CHealth.h"



//----------------------------------------------------------------------------------------------------------------------
void SHealth::Startup()
{
	AddWriteDependencies<CHealth>();
}



//----------------------------------------------------------------------------------------------------------------------
void SHealth::Run(SystemContext const& context)
{
	auto& healthStorage = g_ecs->GetArrayStorage<CHealth>();

	for (auto it = g_ecs->Iterate<CHealth>(context); it.IsValid(); ++it)
	{
		CHealth& health = healthStorage[it];

		health.m_currentHealth += health.m_healthRegen * context.m_deltaSeconds;

		if (health.m_currentHealth > health.m_maxHealth)
		{
			health.m_currentHealth = health.m_maxHealth;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SHealth::Shutdown()
{

}