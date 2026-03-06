// Bradley Christensen - 2022-2026
#include "CProjectile.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Math/RandomNumberGenerator.h"



//----------------------------------------------------------------------------------------------------------------------
CProjectile::CProjectile(void const*)
{

}



//----------------------------------------------------------------------------------------------------------------------
void CProjectile::RollDamageAndEffects()
{
	if (m_hasRolledDamageAndEffects)
	{
		return;
	}

	float critMultiplier = 2.f; // todo: move to gameplay static constants file?
	bool didCrit = false;
	if (m_critComp.has_value())
	{
		AbilityCritComponent& critComp = m_critComp.value();
		float critRoll = g_rng->GetRandomFloatZeroToOne();
		critMultiplier = 2.f + critComp.m_critMulti;
		critComp.m_didCrit = critRoll < critComp.m_critChance;
		didCrit = critComp.m_didCrit;
	}

	if (m_onHitComp.has_value())
	{
		AbilityOnHitComponent& onHitComp = m_onHitComp.value();
		if (onHitComp.m_damageOnHit.has_value())
		{
			AbilityDamageComponent& damageComp = onHitComp.m_damageOnHit.value();
			damageComp.m_damageDone = g_rng->GetRandomFloatInRange(damageComp.m_minDamage, damageComp.m_maxDamage);
			if (didCrit)
			{
				damageComp.m_damageDone *= critMultiplier;
			}
		}

		if (onHitComp.m_burnOnHit.has_value())
		{
			AbilityBurnComponent& burnComp = onHitComp.m_burnOnHit.value();
			burnComp.m_burnDone = burnComp.m_burn;
			if (didCrit)
			{
				burnComp.m_burnDone *= critMultiplier;
			}
		}

		if (onHitComp.m_poisonOnHit.has_value())
		{
			AbilityPoisonComponent& poisonComp = onHitComp.m_poisonOnHit.value();
			poisonComp.m_poisonDone = poisonComp.m_poison;
			if (didCrit)
			{
				poisonComp.m_poisonDone *= critMultiplier;
			}
		}

		if (onHitComp.m_aoeHitOnHit.has_value())
		{
			AbilityAoeHitComponent& aoeHitComp = onHitComp.m_aoeHitOnHit.value();
			if (aoeHitComp.m_damageOnHit.has_value())
			{
				AbilityDamageComponent& aoeDamageComp = aoeHitComp.m_damageOnHit.value();
				aoeDamageComp.m_damageDone = g_rng->GetRandomFloatInRange(aoeDamageComp.m_minDamage, aoeDamageComp.m_maxDamage);
				if (didCrit)
				{
					aoeDamageComp.m_damageDone *= critMultiplier;
				}
			}

			if (aoeHitComp.m_burnOnHit.has_value())
			{
				AbilityBurnComponent& aoeBurnComp = aoeHitComp.m_burnOnHit.value();
				aoeBurnComp.m_burnDone = aoeBurnComp.m_burn;
				if (didCrit)
				{
					aoeBurnComp.m_burnDone *= critMultiplier;
				}
			}

			if (aoeHitComp.m_poisonOnHit.has_value())
			{
				AbilityPoisonComponent& aoePoisonComp = aoeHitComp.m_poisonOnHit.value();
				aoePoisonComp.m_poisonDone = aoePoisonComp.m_poison;
				if (didCrit)
				{
					aoePoisonComp.m_poisonDone *= critMultiplier;
				}
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload CProjectile::GetMainTargetPayload() const
{
	HitPayload result;
	result.m_damage = m_onHitComp.has_value() && m_onHitComp->m_damageOnHit.has_value() ? m_onHitComp->m_damageOnHit->m_damageDone : 0.f;
	result.m_burn = m_onHitComp.has_value() && m_onHitComp->m_burnOnHit.has_value() ? m_onHitComp->m_burnOnHit->m_burnDone : 0.f;
	result.m_poison = m_onHitComp.has_value() && m_onHitComp->m_poisonOnHit.has_value() ? m_onHitComp->m_poisonOnHit->m_poisonDone : 0.f;
	result.m_slowDuration = m_onHitComp.has_value() && m_onHitComp->m_slowOnHit.has_value() ? m_onHitComp->m_slowOnHit->m_duration : 0.f;
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload CProjectile::GetAoeTargetPayload() const
{
	HitPayload result;
	result.m_damage = m_onHitComp.has_value() && m_onHitComp->m_aoeHitOnHit.has_value() && m_onHitComp->m_aoeHitOnHit->m_damageOnHit.has_value() ? m_onHitComp->m_aoeHitOnHit->m_damageOnHit->m_damageDone : 0.f;
	result.m_burn = m_onHitComp.has_value() && m_onHitComp->m_aoeHitOnHit.has_value() && m_onHitComp->m_aoeHitOnHit->m_burnOnHit.has_value() ? m_onHitComp->m_aoeHitOnHit->m_burnOnHit->m_burnDone : 0.f;
	result.m_poison = m_onHitComp.has_value() && m_onHitComp->m_aoeHitOnHit.has_value() && m_onHitComp->m_aoeHitOnHit->m_poisonOnHit.has_value() ? m_onHitComp->m_aoeHitOnHit->m_poisonOnHit->m_poisonDone : 0.f;
	result.m_slowDuration = m_onHitComp.has_value() && m_onHitComp->m_aoeHitOnHit.has_value() && m_onHitComp->m_aoeHitOnHit->m_slowOnHit.has_value() ? m_onHitComp->m_aoeHitOnHit->m_slowOnHit->m_duration : 0.f;
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
void CProjectile::AppendDebugString(std::string& out_string) const
{
	out_string += StringUtils::StringF("Proj Speed: %.1f\n", m_projSpeed);
	if (m_critComp.has_value())
	{
		m_critComp->AppendDebugString(out_string);
	}
	if (m_onHitComp.has_value())
	{
		m_onHitComp->AppendDebugString(out_string);
	}
}