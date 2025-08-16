// Bradley Christensen - 2023
#include "CMovement.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CMovement::CMovement(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_movementSpeed = ParseXmlAttribute(elem, "moveSpeed", m_movementSpeed);
    m_sprintMoveSpeedMultiplier = ParseXmlAttribute(elem, "sprintMoveSpeedMultiplier", m_sprintMoveSpeedMultiplier);
}
