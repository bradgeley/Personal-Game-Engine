// Bradley Christensen - 2022-2025
#include "SHealth.h"
#include "CHealth.h"
#include "Engine/Math/MathUtils.h"



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

		if (health.m_currentHealth <= 0.f)
		{
			// Dead
			health.SetHealthReachedZero(true);
		}
		else
		{
			// Not Dead
			if (!health.GetRegenSuppressed())
			{
				health.m_currentHealth += health.m_healthRegen * context.m_deltaSeconds;
			}
		}

		health.m_currentHealth = MathUtils::Clamp(health.m_currentHealth, 0.f, health.m_maxHealth);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SHealth::Shutdown()
{

}