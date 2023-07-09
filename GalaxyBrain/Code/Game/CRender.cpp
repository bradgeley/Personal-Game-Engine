// Bradley Christensen - 2023
#include "CRender.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Game/SRender.h"



//----------------------------------------------------------------------------------------------------------------------
CRender::CRender(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_layer = ParseXmlAttribute(elem, "layer", m_layer);
    m_scale = ParseXmlAttribute(elem, "scale", m_scale);

    std::string vboName = ParseXmlAttribute(elem, "vbo", "");
    m_vbo = SRender::CreateOrGetVbo(vboName);

    std::string texture = ParseXmlAttribute(elem, "texture", "");
    m_texture = SRender::CreateOrGetTexture(texture);
}
