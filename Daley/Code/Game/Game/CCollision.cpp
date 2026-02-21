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
	SetImmovable(immovable);

	bool singleHash = XmlUtils::ParseXmlAttribute(elem, "singleHash", false);
	SetIsSingleHash(singleHash);
}



//----------------------------------------------------------------------------------------------------------------------
bool CCollision::IsImmovable() const
{
    return (m_collisionFlags & (uint8_t) CollisionFlags::Immovable) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CCollision::GetIsSingleHash() const
{
	return (m_collisionFlags & (uint8_t) CollisionFlags::SingleHash) != 0;
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
void CCollision::SetIsSingleHash(bool singleHash)
{
    if (singleHash)
    {
        m_collisionFlags |= (uint8_t) CollisionFlags::SingleHash;
    }
    else
    {
        m_collisionFlags &= ~((uint8_t) CollisionFlags::SingleHash);
    }
}
