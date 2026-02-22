// Bradley Christensen - 2022-2026
#include "SInput.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Input/InputSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SInput::Startup()
{
    AddReadDependencies<InputSystem>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInput::Run(SystemContext const&)
{
	// Pause
	if (g_input->WasKeyJustPressed(KeyCode::Space))
	{
		NamedProperties args;
		g_eventSystem->FireEvent("TogglePaused", args);
	}
}
