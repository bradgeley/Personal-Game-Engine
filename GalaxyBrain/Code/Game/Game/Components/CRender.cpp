// Bradley Christensen - 2023
#include "CRender.h"

#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CRender::CRender(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_vboIndex = ParseXmlAttribute(elem, "vboIndex", m_vboIndex);
}
