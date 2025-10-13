// Bradley Christensen - 2022-2025
#include "CLifetime.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CLifetime::CLifetime(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

	m_lifetime = XmlUtils::ParseXmlAttribute(elem, "lifetime", m_lifetime);
	m_lifetimeRemaining = m_lifetime;
}
