// Bradley Christensen - 2023
#include "CPlayerController.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Core/EngineCommon.h"



//----------------------------------------------------------------------------------------------------------------------
CPlayerController::CPlayerController(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    UNUSED(elem)
}
