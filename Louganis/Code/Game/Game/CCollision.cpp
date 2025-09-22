// Bradley Christensen - 2022-2025
#include "CCollision.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CCollision::CCollision(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

    m_radius = XmlUtils::ParseXmlAttribute(elem, "radius", m_radius);

	m_offset = XmlUtils::ParseXmlAttribute(elem, "offset", m_offset);

    bool immovable = XmlUtils::ParseXmlAttribute(elem, "immovable", false);
    m_collisionFlags |= immovable ? (uint8_t) CollisionFlags::Immovable : 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CCollision::IsImmovable() const
{
    return (m_collisionFlags & (uint8_t) CollisionFlags::Immovable) != 0;
}
