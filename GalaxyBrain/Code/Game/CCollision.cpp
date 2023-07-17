// Bradley Christensen - 2023
#include "CCollision.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CCollision::CCollision(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

    m_radius = ParseXmlAttribute(elem, "radius", 0.f);
    
    std::string collisionType = ParseXmlAttribute(elem, "type", "");
    if (collisionType == "static")
    {
        m_type = CollisionType::Static;
    }
    else if (collisionType == "mobile")
    {
        m_type = CollisionType::Mobile;
    }

    std::string attachType = ParseXmlAttribute(elem, "attachType", "");
    if (attachType == "canAttach")
    {
        m_attachType = AttachmentType::CanAttach;
    }
    else if (attachType == "canHaveAttachedEntities")
    {
        m_attachType = AttachmentType::CanHaveAttachedEntities;
    }
}
