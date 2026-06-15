// Bradley Christensen - 2022-2026
#include "SInput.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Input/InputSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SInput::Startup()
{
    AddReadDependencies<InputSystem, EventSystem>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInput::Run(SystemContext const&) const
{
	// Read Dependencies
	// g_input
	// g_eventSystem

	// Pause
	if (g_input->WasKeyJustPressed(KeyCode::Space))
	{
		NamedProperties args;
		g_eventSystem->FireEvent("TogglePaused", args);
	}
}
