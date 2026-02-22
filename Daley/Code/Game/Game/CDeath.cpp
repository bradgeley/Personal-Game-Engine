// Bradley Christensen - 2022-2026
#include "CDeath.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CDeath::CDeath(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

	m_deathAnimationName	= XmlUtils::ParseXmlAttribute(elem, "deathAnimation", m_deathAnimationName);
	m_corpseDurationSeconds = XmlUtils::ParseXmlAttribute(elem, "corpseDurationSeconds", 1.f);
}



//----------------------------------------------------------------------------------------------------------------------
bool CDeath::GetIsDead() const
{
	return GetDiedThisFrame() || (m_deathFlags & static_cast<uint8_t>(DeathTags::IsDead)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CDeath::GetDiedThisFrame() const
{
	return (m_deathFlags & static_cast<uint8_t>(DeathTags::DiedThisFrame)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
void CDeath::SetIsDead(bool isDead)
{
	if (isDead)
	{
		m_deathFlags |= static_cast<uint8_t>(DeathTags::IsDead);
	}
	else
	{
		m_deathFlags &= ~static_cast<uint8_t>(DeathTags::IsDead);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CDeath::SetDiedThisFrame(bool diedThisFrame)
{
	if (diedThisFrame)
	{
		m_deathFlags |= static_cast<uint8_t>(DeathTags::DiedThisFrame);
	}
	else
	{
		m_deathFlags &= ~static_cast<uint8_t>(DeathTags::DiedThisFrame);
	}
}
