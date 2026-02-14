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

	bool mustStayOnPath = XmlUtils::ParseXmlAttribute(elem, "mustStayOnPath", false);
	m_collisionFlags |= mustStayOnPath ? (uint8_t) CollisionFlags::MustStayOnPath : 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CCollision::IsImmovable() const
{
    return (m_collisionFlags & (uint8_t) CollisionFlags::Immovable) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CCollision::GetMustStayOnPath() const
{
	return (m_collisionFlags & (uint8_t) CollisionFlags::MustStayOnPath) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CCollision::IsCollisionEnabled() const
{
	return (m_collisionFlags & (uint8_t) CollisionFlags::Enabled) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
void CCollision::SetCollisionEnabled(bool enabled)
{
    if (enabled)
    {
        m_collisionFlags |= (uint8_t) CollisionFlags::Enabled;
    }
    else
    {
        m_collisionFlags &= ~((uint8_t) CollisionFlags::Enabled);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CCollision::SetImmovable(bool immovable)
{
    if (immovable)
    {
        m_collisionFlags |= (uint8_t) CollisionFlags::Immovable;
    }
    else
    {
        m_collisionFlags &= ~((uint8_t) CollisionFlags::Immovable);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CCollision::SetMustStayOnPath(bool mustStayOnPath)
{
    if (mustStayOnPath)
    {
        m_collisionFlags |= (uint8_t) CollisionFlags::MustStayOnPath;
    }
    else
    {
        m_collisionFlags &= ~((uint8_t) CollisionFlags::MustStayOnPath);
	}
}
