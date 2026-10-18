// Bradley Christensen - 2022-2026
#include "CHealth.h"
#include "EntityDebugContext.h"
#include "HitPayload.h"
#include "GameCommon.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/TagQuery.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CHealth::CHealth(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_maxHealth = XmlUtils::ParseXmlAttribute(elem, "maxHealth", m_maxHealth);
	m_healthRegen = XmlUtils::ParseXmlAttribute(elem, "healthRegen", m_healthRegen);
	m_currentHealth = m_maxHealth;
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::TakeDamage(float damage)
{
	float effectiveDamage = damage * (1.f - m_physicalResistance) * m_damageTakenMultiplier;

	m_currentHealth -= effectiveDamage;

	if (m_currentHealth <= 0.f)
	{
		m_currentHealth = 0.f;
		SetHealthReachedZero(true);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::TakePayload(HitPayload const& payload)
{
	if (GetIsProtected() || GetIsImmune())
	{
		return;
	}

	float burnDealt = payload.m_burn * (1.f - m_burnResistance) * (GetIsSuppressingBurn() ? StaticGameSettings::s_burnSuppressionRatio : 1.f);
	float poisonDealt = payload.m_poison * (1.f - m_poisonResistance) * (GetIsSuppressingPoison() ? StaticGameSettings::s_poisonSuppressionRatio : 1.f);

	m_currentBurn += burnDealt;
	m_currentPoison += poisonDealt;
	m_currentVulnerability += payload.m_vulnerability;

	TakeDamage(payload.m_physical);
}



//----------------------------------------------------------------------------------------------------------------------
float CHealth::GetHealthFraction() const
{
	return m_currentHealth / m_maxHealth;
}



//----------------------------------------------------------------------------------------------------------------------
float CHealth::GetTotalBurnDamageRemaining() const
{
	return m_currentBurn / StaticGameSettings::s_burnDecayK;
}



//----------------------------------------------------------------------------------------------------------------------
float CHealth::GetBurnSaturation() const
{
	float saturation = GetTotalBurnDamageRemaining() / m_currentHealth;
	saturation = MathUtils::Clamp(saturation, 0.f, 1.f);
	return saturation;
}



//----------------------------------------------------------------------------------------------------------------------
float CHealth::GetPoisonSaturation() const
{
	float saturation = (m_currentPoison - m_healthRegen) / (0.25f * m_currentHealth);
	saturation = MathUtils::Clamp(saturation, 0.f, 1.f);
	return saturation;
} 



//----------------------------------------------------------------------------------------------------------------------
float CHealth::GetVulnerabilitySaturation() const
{
	float saturation = MathUtils::RangeMapClamped(m_damageTakenMultiplier, 1.f, 2.f, 0.f, 1.f);
	return saturation;
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::MatchesTagQuery(TagQuery const& query) const
{
	return query.Resolve(m_healthFlags);
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::GetIsImmune() const
{
	return m_immunityDurationLeft > 0.f;
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::GetRegenSuppressed() const
{
	return (m_healthFlags & static_cast<uint8_t>(HealthFlags::RegenSuppressed)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::GetCanBeProtected() const
{
	return (m_healthFlags & static_cast<uint8_t>(HealthFlags::CanBeProtected)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::GetHealthReachedZero() const
{
	return (m_healthFlags & static_cast<uint8_t>(HealthFlags::HealthReachedZero)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::GetIsProtected() const
{
	if (!GetCanBeProtected())
	{
		return false;
	}
	return (m_healthFlags & static_cast<uint8_t>(HealthFlags::IsProtected)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::GetIsSuppressingBurn() const
{
	return (m_healthFlags & static_cast<uint8_t>(HealthFlags::IsSuppressingBurn)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::GetIsSuppressingPoison() const
{
	return (m_healthFlags & static_cast<uint8_t>(HealthFlags::IsSuppressingPoison)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::SetImmunityDuration(float duration)
{
	if (GetIsImmune())
	{
		// Can't become immune if already immune, for balance reasons
		return;
	}
	m_immunityDurationLeft = duration;
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::SetRegenSuppressed(bool regenSuppressed)
{
	if (regenSuppressed)
	{
		m_healthFlags |= static_cast<uint8_t>(HealthFlags::RegenSuppressed);
	}
	else
	{
		m_healthFlags &= ~static_cast<uint8_t>(HealthFlags::RegenSuppressed);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::SetCanBeProtected(bool canBeProtected)
{
	if (canBeProtected)
	{
		m_healthFlags |= static_cast<uint8_t>(HealthFlags::CanBeProtected);
	}
	else
	{
		m_healthFlags &= ~static_cast<uint8_t>(HealthFlags::CanBeProtected);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::SetHealthReachedZero(bool reachedZero)
{
	if (reachedZero)
	{
		m_healthFlags |= static_cast<uint8_t>(HealthFlags::HealthReachedZero);
	}
	else
	{
		m_healthFlags &= ~static_cast<uint8_t>(HealthFlags::HealthReachedZero);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::SetIsProtected(bool isProtected)
{
	if (isProtected)
	{
		m_healthFlags |= static_cast<uint8_t>(HealthFlags::IsProtected);
	}
	else
	{
		m_healthFlags &= ~static_cast<uint8_t>(HealthFlags::IsProtected);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::SetIsSuppressingBurn(bool isSuppressingBurn)
{
	if (isSuppressingBurn)
	{
		m_healthFlags |= static_cast<uint8_t>(HealthFlags::IsSuppressingBurn);
	}
	else
	{
		m_healthFlags &= ~static_cast<uint8_t>(HealthFlags::IsSuppressingBurn);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::SetIsSuppressingPoison(bool isSuppressingPoison)
{
	if (isSuppressingPoison)
	{
		m_healthFlags |= static_cast<uint8_t>(HealthFlags::IsSuppressingPoison);
	}
	else
	{
		m_healthFlags &= ~static_cast<uint8_t>(HealthFlags::IsSuppressingPoison);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::AppendDebugString(EntityDebugContext& context) const
{
	context.m_debugString += StringUtils::StringF("Health: %.1f/%.1f\n", m_currentHealth, m_maxHealth);
	if (m_healthRegen > 0.f)
	{
		context.m_debugString += StringUtils::StringF("Health Regen: %.1f\n", m_healthRegen);
	}
	if (m_currentBurn > 0.f)
	{
		context.m_debugString += StringUtils::StringF("Burn: %.1f\n", m_currentBurn);
	}
	if (m_currentPoison > 0.f)
	{
		context.m_debugString += StringUtils::StringF("Poison: %.1f\n", m_currentPoison);
	}
	if (m_currentVulnerability > 0.f)
	{
		context.m_debugString += StringUtils::StringF("Vuln: %.1f (x%.2f)\n", m_currentVulnerability, m_damageTakenMultiplier);
	}
}