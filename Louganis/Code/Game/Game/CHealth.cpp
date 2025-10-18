// Bradley Christensen - 2022-2025
#include "CHealth.h"
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
}



//----------------------------------------------------------------------------------------------------------------------
bool CHealth::GetIsDead() const
{
	return m_currentHealth <= 0.f;
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
