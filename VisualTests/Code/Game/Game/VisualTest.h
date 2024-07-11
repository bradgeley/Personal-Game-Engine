// Bradley Christensen - 2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Math/RandomNumberGenerator.h"



//----------------------------------------------------------------------------------------------------------------------
// Any mode that tests something that needs visuals
//
class VisualTest : public EngineSubsystem
{
public:

	virtual void DisplayHelpMessage() const;

	std::string m_name = "Unnamed Visual Test";
	RandomNumberGenerator m_rng;
	Camera m_camera;
};

