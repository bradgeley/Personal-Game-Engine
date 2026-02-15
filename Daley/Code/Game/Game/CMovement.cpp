// Bradley Christensen - 2022-2025
#include "CMovement.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CMovement::CMovement(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_movementSpeed = XmlUtils::ParseXmlAttribute(elem, "moveSpeed", m_movementSpeed);

	bool isConstrainedToPath = XmlUtils::ParseXmlAttribute(elem, "isConstrainedToPath", false);
	SetIsConstrainedToPath(isConstrainedToPath);
}



//----------------------------------------------------------------------------------------------------------------------
bool CMovement::GetIsTeleporting() const
{
	return (m_movementFlags & static_cast<uint8_t>(MovementFlags::IsTeleporting)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CMovement::GetIsConstrainedToPath() const
{
    return (m_movementFlags & static_cast<uint8_t>(MovementFlags::ConstrainedToPath)) != 0;
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
