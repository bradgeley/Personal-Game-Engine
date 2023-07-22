// Bradley Christensen - 2023
#include "CCamera.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CCamera::CCamera(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_baseDims = ParseXmlAttribute(elem, "dims", m_baseDims);
    m_snappiness = ParseXmlAttribute(elem, "snap", m_snappiness);
    m_minZoom = ParseXmlAttribute(elem, "minZoom", m_minZoom);

    // Nothing should change here, just a test!
    m_camera.DefineGameSpace(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
}
