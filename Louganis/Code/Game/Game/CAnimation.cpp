// Bradley Christensen - 2022-2025
#include "CAnimation.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CAnimation::CAnimation(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_spriteSheetName = XmlUtils::ParseXmlAttribute(elem, "spriteSheet", Name::s_invalidName);
}
