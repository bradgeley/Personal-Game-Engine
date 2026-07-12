// Bradley Christensen - 2022-2026
#include "CMovement.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"
#include "GameCommon.h"



//----------------------------------------------------------------------------------------------------------------------
CMovement::CMovement(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_movementSpeed = XmlUtils::ParseXmlAttribute(elem, "moveSpeed", m_movementSpeed);
	m_rotationSpeedDegPerSec = XmlUtils::ParseXmlAttribute(elem, "rotationSpeed", m_rotationSpeedDegPerSec);

	bool isConstrainedToPath = XmlUtils::ParseXmlAttribute(elem, "pathOnly", false);
	SetIsConstrainedToPath(isConstrainedToPath);
}



//----------------------------------------------------------------------------------------------------------------------
void CMovement::AppendDebugString(std::string& out_string, bool isSlowed) const
{
	float totalMovementSpeed = m_movementSpeed * m_movementSpeedMultiplier * (isSlowed ? StaticGameSettings::s_slowStatusTimeDilation : 1.f);
    out_string += StringUtils::StringF("Speed: %.1f (Base:%.1f)", totalMovementSpeed, m_movementSpeed);
    if (isSlowed)
    {
		out_string += StringUtils::StringF(" (Slowed: %.1f)", StaticGameSettings::s_slowStatusTimeDilation);
    }
	out_string += "\n";
}

 

//----------------------------------------------------------------------------------------------------------------------
bool CMovement::GetIsConstrainedToPath() const
{
    return (m_movementFlags & static_cast<uint8_t>(MovementFlags::ConstrainedToPath)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
void CMovement::SetIsConstrainedToPath(bool isConstrainedToPath)
{
    if (isConstrainedToPath)
    {
        m_movementFlags |= static_cast<uint8_t>(MovementFlags::ConstrainedToPath);
    }
    else
    {
        m_movementFlags &= ~static_cast<uint8_t>(MovementFlags::ConstrainedToPath);
	}
}
