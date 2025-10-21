// Bradley Christensen - 2022-2025
#include "CMovement.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CMovement::CMovement(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_movementSpeed = XmlUtils::ParseXmlAttribute(elem, "moveSpeed", m_movementSpeed);
    m_sprintMoveSpeedMultiplier = XmlUtils::ParseXmlAttribute(elem, "sprintMoveSpeedMultiplier", m_sprintMoveSpeedMultiplier);
}



//----------------------------------------------------------------------------------------------------------------------
bool CMovement::GetIsSprinting() const
{
	return (m_movementFlags & static_cast<uint8_t>(MovementFlags::IsSprinting)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CMovement::GetIsTeleporting() const
{
	return (m_movementFlags & static_cast<uint8_t>(MovementFlags::IsTeleporting)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
void CMovement::SetIsSprinting(bool isSprinting)
{
    if (isSprinting)
    {
        m_movementFlags |= static_cast<uint8_t>(MovementFlags::IsSprinting);
    }
    else
    {
        m_movementFlags &= ~static_cast<uint8_t>(MovementFlags::IsSprinting);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CMovement::SetIsTeleporting(bool isTeleporting)
{
    if (isTeleporting)
    {
        m_movementFlags |= static_cast<uint8_t>(MovementFlags::IsTeleporting);
    }
    else
    {
        m_movementFlags &= ~static_cast<uint8_t>(MovementFlags::IsTeleporting);
	}
}
