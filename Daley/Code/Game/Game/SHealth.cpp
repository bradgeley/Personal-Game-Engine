// Bradley Christensen - 2022-2026
#include "SHealth.h"
#include "CHealth.h"
#include "GameCommon.h"
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

		if (health.GetHealthReachedZero())
		{
			// Been dead
			continue;
		}

		// Regen/Poison
		float regenAmount = health.GetRegenSuppressed() ? 0.f : health.m_healthRegen;
		float poisonAmount = health.m_currentPoison;
		float healthPerSecond = regenAmount - poisonAmount;

		health.m_currentHealth += healthPerSecond * context.m_deltaSeconds;

		// Burn
		if (health.m_currentBurn > 0.f)
		{
			float decayFactor = MathUtils::ExpF(-StaticGameSettings::s_burnDecayK * context.m_deltaSeconds);
			float oldBurn = health.m_currentBurn;
			float newBurn = oldBurn * decayFactor;
			float damageTakenFromBurn = (oldBurn * StaticGameSettings::s_oneOverBurnDecayK) * (1.f - decayFactor);
			health.m_currentHealth -= damageTakenFromBurn;
			health.m_currentBurn = newBurn;
		}

		if (health.m_currentHealth <= 0.f)
		{
			health.SetHealthReachedZero(true);
		}

		health.m_currentHealth = MathUtils::Clamp(health.m_currentHealth, 0.f, health.m_maxHealth);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SHealth::Shutdown()
{

}