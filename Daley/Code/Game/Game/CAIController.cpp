// Bradley Christensen - 2022-2026
#include "CAIController.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CAIController::CAIController(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	bool isWiggly = XmlUtils::ParseXmlAttribute(elem, "isWiggly", false);
	SetIsMovementWiggly(isWiggly);
}



//----------------------------------------------------------------------------------------------------------------------
bool CAIController::GetIsMovementWiggly() const
{
    return (m_aiMovementFlags & static_cast<uint8_t>(AIMovementFlags::IsWiggly)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
void CAIController::SetIsMovementWiggly(bool isWiggly)
{
    if (isWiggly)
    {
        m_aiMovementFlags |= static_cast<uint8_t>(AIMovementFlags::IsWiggly);
    }
    else
    {
        m_aiMovementFlags &= ~static_cast<uint8_t>(AIMovementFlags::IsWiggly);
    }
}

