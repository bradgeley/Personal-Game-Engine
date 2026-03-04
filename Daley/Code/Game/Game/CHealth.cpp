// Bradley Christensen - 2022-2026
#include "CHealth.h"
#include "HitPayload.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CHealth::CHealth(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_maxHealth = XmlUtils::ParseXmlAttribute(elem, "maxHealth", m_maxHealth);
	m_healthRegen = XmlUtils::ParseXmlAttribute(elem, "healthRegen", m_healthRegen);
	m_currentHealth = m_maxHealth;

	bool neverShowHealthBar = XmlUtils::ParseXmlAttribute(elem, "neverShowHealthBar", false);
	SetNeverShowHealthBar(neverShowHealthBar);

	bool isTargetable = XmlUtils::ParseXmlAttribute(elem, "isTargetable", false);
	SetIsTargetable(isTargetable);
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::TakeDamage(float damage)
{
	m_currentHealth -= damage;

	if (m_currentHealth <= 0.f)
	{
		m_currentHealth = 0.f;
		SetHealthReachedZero(true);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::TakePayload(HitPayload const& payload)
{
	m_currentBurn += payload.m_burn;
	m_currentPoison += payload.m_poison;

	TakeDamage(payload.m_damage);
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::MatchesTagQuery(TagQuery query) const
{
	return query.Resolve(m_healthFlags);
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::GetHealthReachedZero() const
{
	return (m_healthFlags & static_cast<uint8_t>(HealthFlags::HealthReachedZero)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::GetIsInvincible() const
{
	return (m_healthFlags & static_cast<uint8_t>(HealthFlags::Invincible)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::GetNeverShowHealthBar() const
{
	return (m_healthFlags & static_cast<uint8_t>(HealthFlags::NeverShowHealthBar)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::GetRegenSuppressed() const
{
	return (m_healthFlags & static_cast<uint8_t>(HealthFlags::RegenSuppressed)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::GetIsTargetable() const
{
	return (m_healthFlags & static_cast<uint8_t>(HealthFlags::IsTargetable)) != 0;
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
void CHealth::SetInvincible(bool isInvincible)
{
	if (isInvincible)
	{
		m_healthFlags |= static_cast<uint8_t>(HealthFlags::Invincible);
	}
	else
	{
		m_healthFlags &= ~static_cast<uint8_t>(HealthFlags::Invincible);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::SetNeverShowHealthBar(bool neverShowHealthBar)
{
	if (neverShowHealthBar)
	{
		m_healthFlags |= static_cast<uint8_t>(HealthFlags::NeverShowHealthBar);
	}
	else
	{
		m_healthFlags &= ~static_cast<uint8_t>(HealthFlags::NeverShowHealthBar);
	}
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
void CHealth::SetIsTargetable(bool isTargetable)
{
	if (isTargetable)
	{
		m_healthFlags |= static_cast<uint8_t>(HealthFlags::IsTargetable);
	}
	else
	{
		m_healthFlags &= ~static_cast<uint8_t>(HealthFlags::IsTargetable);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CHealth::AppendDebugString(std::string& out_string) const
{
	out_string += StringUtils::StringF("Health: %.1f/%.1f\n", m_currentHealth, m_maxHealth);
	out_string += StringUtils::StringF("Health Regen: %.1f\n", m_healthRegen);
	out_string += StringUtils::StringF("Current Burn: %.1f\n", m_currentBurn);
	out_string += StringUtils::StringF("Current Poison: %.1f\n", m_currentPoison);
}