// Bradley Christensen - 2022-2026
#include "CMovement.h"
#include "EntityDebugContext.h"
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
void CMovement::AppendDebugString(EntityDebugContext& context) const
{
	float totalMovementSpeed = m_movementSpeed * m_movementSpeedMultiplier * (context.m_isSlowed ? static_cast<float>(StaticGameSettings::s_slowStatusTimeDilation) : 1.f);
	totalMovementSpeed *= (context.m_isHasted ? static_cast<float>(StaticGameSettings::s_hasteStatusTimeDilation) : 1.f);
    context.m_debugString += StringUtils::StringF("Speed: %.1f (Base:%.1f)", totalMovementSpeed, m_movementSpeed);
    if (context.m_isSlowed)
    {
		context.m_debugString += StringUtils::StringF(" (Slowed: %.1f)", static_cast<float>(StaticGameSettings::s_slowStatusTimeDilation));
    }
    if (context.m_isHasted)
    {
        context.m_debugString += StringUtils::StringF(" (Hasted: %.1f)", static_cast<float>(StaticGameSettings::s_hasteStatusTimeDilation));
    }
	context.m_debugString += "\n";
}

 

//----------------------------------------------------------------------------------------------------------------------
bool CMovement::GetIsConstrainedToPath() const
{
    return (m_movementFlags & static_cast<uint8_t>(MovementFlags::ConstrainedToPath)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CMovement::GetMovedThisFrame() const
{
	return (m_movementFlags & static_cast<uint8_t>(MovementFlags::MovedThisFrame)) != 0;
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



//----------------------------------------------------------------------------------------------------------------------
void CMovement::SetMovedThisFrame(bool movedThisFrame)
{
    if (movedThisFrame)
    {
        m_movementFlags |= static_cast<uint8_t>(MovementFlags::MovedThisFrame);
    }
    else
    {
        m_movementFlags &= ~static_cast<uint8_t>(MovementFlags::MovedThisFrame);
    }
}
