// Bradley Christensen - 2022-2026
#include "SInput.h"
#include "SCInputSystem.h"
#include "SCGame.h"
#include "Game.h"
#include "Engine/Input/InputSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SInput::Startup()
{
	AddReadDependencies<SCInputSystem>();
    AddWriteDependencies<SCGame>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInput::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCInputSystem const& input = context.GetSingletonConst<SCInputSystem>();
	InputSystem const& inputSystem = *input.GetInputSystem();

	// Write Dependencies
	SCGame& game = context.GetSingleton<SCGame>();
	Game& gameInstance = *game.m_game;

	// Pause
	if (inputSystem.WasKeyJustPressed(KeyCode::Space))
	{
		gameInstance.TogglePaused();
	}
}
