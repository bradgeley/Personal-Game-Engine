// Bradley Christensen - 2022-2026
#include "SRunModifier.h"
#include "GameState.h"
#include "SCGameState.h"
#include "SCRunData.h"
#include "SCInputSystem.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Input/InputSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SRunModifier::Startup()
{
	AddWriteDependencies<SCRunData, SCGameState>();
	AddReadDependencies<SCInputSystem>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRunModifier::Run(SystemContext const& context) const
{
	// Write Dependencies
	SCRunData& scRunData = context.GetSingleton<SCRunData>();
	RunData& runData = *scRunData.m_data;
	SCGameState& scGameState = context.GetSingleton<SCGameState>();
	GameState& gameState = *scGameState.m_gameState;

	// Read Dependencies
	SCInputSystem const	& scInput = context.GetSingletonConst<SCInputSystem>();
	InputSystem const& input = *scInput.GetInputSystem();

	if (runData.m_numModifierChoicesRemaining == 0)
	{
		return;
	}

	if (runData.m_numActiveModifierChoices == 0)
	{
		runData.GenerateModifierChoices();
		if (runData.m_numActiveModifierChoices > 0 && !gameState.IsPaused())
		{
			gameState.TogglePaused();
		}
		return;
	}

	int choice = -1;
	if (input.WasKeyJustPressed('1'))
	{
		choice = 0;
	}
	else if (input.WasKeyJustPressed('2'))
	{
		choice = 1;
	}
	else if (input.WasKeyJustPressed('3'))
	{
		choice = 2;
	}

	if (choice == -1)
	{
		return;
	}

	if (runData.m_modifierChoices[choice] != nullptr)
	{
		// Todo: level up existing modifier

		RunModifier* newModifier = runData.m_modifierChoices[choice]->MakeModifierInstance();
		runData.m_activeRunModifiers.push_back(newModifier);

		newModifier->Apply(context);
		runData.m_numModifierChoicesCompleted++;
		runData.m_numModifierChoicesRemaining--;
		runData.m_numActiveModifierChoices = 0;
		gameState.TogglePaused();
	}
}
