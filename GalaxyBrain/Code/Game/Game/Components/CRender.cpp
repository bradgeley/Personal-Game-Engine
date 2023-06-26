// Bradley Christensen - 2023
#include "CRender.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Renderer/Texture.h"



//----------------------------------------------------------------------------------------------------------------------
CRender::CRender(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_vboIndex = ParseXmlAttribute(elem, "vboIndex", m_vboIndex);
    m_layer = ParseXmlAttribute(elem, "layer", m_layer);

    std::string texture = ParseXmlAttribute(elem, "texture", "");
    if (!texture.empty())
    {
        m_texture = new Texture();
        m_texture->LoadFromImageFile(texture.c_str());
    }
}
