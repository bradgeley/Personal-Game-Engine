// Bradley Christensen - 2023
#include "CCamera.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CCamera::CCamera(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

    m_tileHeight = ParseXmlAttribute(elem, "tileHeight", m_tileHeight);
}
