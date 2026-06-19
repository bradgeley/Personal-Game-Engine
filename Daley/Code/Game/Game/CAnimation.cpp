// Bradley Christensen - 2022-2026
#include "CAnimation.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
static std::string s_defaultAnimName = "idle";



//----------------------------------------------------------------------------------------------------------------------
CAnimation::CAnimation(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_spriteSheetName = XmlUtils::ParseXmlAttribute(elem, "spriteSheet", Name::Invalid);
    m_defaultAnimationName = XmlUtils::ParseXmlAttribute(elem, "defaultAnim", Name(s_defaultAnimName));
}



//----------------------------------------------------------------------------------------------------------------------
CAnimation::~CAnimation()
{
    if (g_assetManager && m_gridSpriteSheet != AssetID::Invalid)
    {
        g_assetManager->Release(m_gridSpriteSheet);
        m_gridSpriteSheet = AssetID::Invalid; // Prevent multiple destructors from releasing the same sprite sheet multiple times. Todo: figure out why this can happen.
    }
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
