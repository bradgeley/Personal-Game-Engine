// Bradley Christensen - 2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Rgba8.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
// THE Dev Console
//
extern class DevConsole* g_devConsole;



//----------------------------------------------------------------------------------------------------------------------
struct DevConsoleConfig
{
	float m_openCloseAnimationSpeed = 2.f;
	Rgba8 m_backgroundTint = Rgba8(50,50,50,225);
};



//----------------------------------------------------------------------------------------------------------------------
// Dev Console
//
// Debug interface for calling functions in the game and engine.
//
class DevConsole : public EngineSubsystem
{
public:

	DevConsole(DevConsoleConfig const& config);
	virtual ~DevConsole() override = default;

	void Startup() override;
	void Update(float deltaSeconds) override;
	void Render() const override;
	void Shutdown() override;

protected:

	// Event Handlers - steals input from the game if the dev console is registered as a subsystem BEFORE InputSystem
	bool HandleChar(NamedProperties& args);
	bool HandleKeyDown(NamedProperties& args);
	bool HandleKeyUp(NamedProperties& args);
	bool HandleMouseButtonDown(NamedProperties& args);
	bool HandleMouseButtonUp(NamedProperties& args);
	bool HandleMouseWheel(NamedProperties& args);

protected:
	
	DevConsoleConfig const m_config;

	bool m_isShowing = false;
	Camera m_camera;

private:

	// Open/Close animation state. If 1.f, dev console is "closed"
	float m_openCloseAnimationFraction = 1.f;
};


