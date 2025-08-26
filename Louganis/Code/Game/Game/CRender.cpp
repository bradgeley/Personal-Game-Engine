// Bradley Christensen - 2022-2025
#include "CRender.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CRender::CRender(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_scale = XmlUtils::ParseXmlAttribute(elem, "scale", m_scale);
}
