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
}