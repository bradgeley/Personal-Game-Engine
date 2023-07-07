﻿// Bradley Christensen - 2023
#include "CCamera.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CCamera::CCamera(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_baseDims = ParseXmlAttribute(elem, "dims", m_baseDims);
    m_snappiness = ParseXmlAttribute(elem, "snap", m_snappiness);
    m_minZoom = ParseXmlAttribute(elem, "minZoom", m_minZoom);
}
