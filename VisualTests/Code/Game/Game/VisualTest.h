// Bradley Christensen - 2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Renderer/Camera.h"



//----------------------------------------------------------------------------------------------------------------------
// Any mode that tests something that needs visuals
//
class VisualTest : public EngineSubsystem
{
public:

	virtual void DisplayHelpMessage() const;

	Camera m_camera;
};

