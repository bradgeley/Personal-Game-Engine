// Bradley Christensen - 2023
#include "CCamera.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CCamera::CCamera(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

    m_baseOrthoHeight = XmlUtils::ParseXmlAttribute(elem, "tileHeight", m_baseOrthoHeight);
    m_minZoom = XmlUtils::ParseXmlAttribute(elem, "minZoom", m_minZoom);
    m_maxZoom = XmlUtils::ParseXmlAttribute(elem, "maxZoom", m_maxZoom);
    m_zoomMultiplier = XmlUtils::ParseXmlAttribute(elem, "zoomMultiplier", m_zoomMultiplier);
}
