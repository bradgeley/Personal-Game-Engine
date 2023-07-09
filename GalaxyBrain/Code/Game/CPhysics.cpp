// Bradley Christensen - 2023
#include "CPhysics.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CPhysics::CPhysics(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

    m_mass = ParseXmlAttribute(elem, "mass", 0.f);
}
