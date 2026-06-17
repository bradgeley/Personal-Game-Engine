// Bradley Christensen - 2022-2026
#include "SInput.h"
#include "SCInputSystem.h"
#include "SCEventSystem.h"
#include "Engine/Core/NamedProperties.h"



//----------------------------------------------------------------------------------------------------------------------
void SInput::Startup()
{
    AddWriteAllDependencies(); // Currently, any event firing requires write all dependencies, as events can result in anything.
}



//----------------------------------------------------------------------------------------------------------------------
void SInput::Run(SystemContext const& context) const
{
	// Read Dependencies
	InputSystem& input = *context.GetSingleton<SCInputSystem>().m_inputSystem;
	EventSystem& eventSystem = *context.GetSingleton<SCEventSystem>().m_eventSystem;

	// Pause
	if (input.WasKeyJustPressed(KeyCode::Space))
	{
		NamedProperties args;
		eventSystem.FireEvent("TogglePaused", args);
	}
}
