// Bradley Christensen - 2022-2025
#include "CAnimation.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
static std::string s_defaultAnimName = "idle";



//----------------------------------------------------------------------------------------------------------------------
CAnimation::CAnimation(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_spriteSheetName = XmlUtils::ParseXmlAttribute(elem, "spriteSheet", Name::Invalid);

    static Name s_defaultAnimName("idle");
    m_defaultAnimationName = XmlUtils::ParseXmlAttribute(elem, "defaultAnim", Name(s_defaultAnimName));
}



//----------------------------------------------------------------------------------------------------------------------
bool CAnimation::PlayAnimation(PlayAnimationRequest const& request, bool force /*= false*/)
{
    if (!force && m_pendingAnimRequest.m_priority > request.m_priority)
    {
        return false;
    }

	m_pendingAnimRequest = request;

    return true;
}
