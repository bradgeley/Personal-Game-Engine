// Bradley Christensen - 2022-2026
#include "CCollision.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CCollision::CCollision(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

    m_radius = XmlUtils::ParseXmlAttribute(elem, "radius", m_radius);

    // Everything forced single hash now, for perf to get MORE enemies
	SetIsSingleHash(true);

	Name collisionObjectType = XmlUtils::ParseXmlAttribute(elem, "type", Name());
    m_collisionChannel = CollisionChannelUtils::GetCollisionChannelByName(collisionObjectType);
}



//----------------------------------------------------------------------------------------------------------------------
bool CCollision::GetIsSingleHash() const
{
	return (m_collisionFlags & (uint8_t) CollisionFlags::SingleHash) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CCollision::IsCollisionEnabled() const
{
	return ((m_collisionFlags & (uint8_t) CollisionFlags::Enabled) != 0) || (m_collisionChannel == CollisionChannel::NoCollision);
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
