// Bradley Christensen - 2022-2026
#include "GameFlow.h"
#include "GameOverState.h"
#include "MainMenuState.h"
#include "RunState.h"
#include "TowerDefenseState.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Events/EventSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::Startup()
{
	g_eventSystem->SubscribeMethod("PushState", this, &GameFlow::PushStateEvent);
	g_eventSystem->SubscribeMethod("PopState", this, &GameFlow::PopStateEvent);
	g_eventSystem->SubscribeMethod("ChangeState", this, &GameFlow::ChangeStateEvent);
	g_eventSystem->SubscribeMethod("GameFlowDump", this, &GameFlow::GameFlowDump);

	PushState("MainMenu");
}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::Shutdown()
{
	g_eventSystem->UnsubscribeMethod("PushState", this, &GameFlow::PushStateEvent);
	g_eventSystem->UnsubscribeMethod("PopState", this, &GameFlow::PopStateEvent);
	g_eventSystem->UnsubscribeMethod("ChangeState", this, &GameFlow::ChangeStateEvent);
	g_eventSystem->UnsubscribeMethod("GameFlowDump", this, &GameFlow::GameFlowDump);

	while (m_states.empty() == false)
	{
		PopState();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::BeginFrame()
{
	HandlePendingTransition();
}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::Update(float deltaSeconds)
{
	if (m_states.empty())
	{
		return;
	}

	m_states[m_states.size() - 1]->Update(deltaSeconds);
}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::Render() const
{
	if (m_states.empty())
	{
		return;
	}

	m_states[m_states.size() - 1]->Render();
}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::EndFrame()
{

}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::PushState(Name state)
{
	DevConsoleUtils::Log(Rgba8::Yellow, "Entering state: %s", state.ToCStr());

	if (state == "MainMenu")
	{
		m_states.push_back(new MainMenuState());
	}
	else if (state == "TowerDefense")
	{
		m_states.push_back(new TowerDefenseState());
	}
	else if (state == "GameOver")
	{
		m_states.push_back(new GameOverState());
	}
	else if (state == "Run")
	{
		m_states.push_back(new RunState());
	}
	else
	{
		ERROR_AND_DIE("GameFlow::PushState: Unknown state");
	}

	m_states.back()->Enter();
}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::PopState()
{
	ASSERT_OR_DIE(m_states.empty() == false, "Cannot pop state when no states are present.");

	GameState* top = m_states.back();
	m_states.pop_back();

	DevConsoleUtils::Log(Rgba8::Yellow, "Exiting state: %s", top->m_name.ToCStr());

	top->Exit();
	delete top;
}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::PopToState(Name state)
{
	bool found = false;
	for (auto& gameState : m_states)
	{
		if (gameState->m_name == state)
		{
			found = true;
			break;
		}
	}

	ASSERT_OR_DIE(found, StringUtils::StringF("Pop to state: %s not found.", state.ToCStr()));

	while (m_states[m_states.size() - 1]->m_name != state)
	{
		PopState();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::HandlePendingTransition()
{
	switch (m_pendingTransition.m_type)
	{
		case StateTransitionType::Push:
		{
			PushState(m_pendingTransition.m_toState);
			break;
		}
		case StateTransitionType::Pop:
		{
			if (m_pendingTransition.m_toState == Name::Invalid)
			{
				PopState();
			}
			else
			{
				PopToState(m_pendingTransition.m_toState);
			}
			break;
		}
		case StateTransitionType::Change:
		{
			PopState();
			PushState(m_pendingTransition.m_toState);
			break;
		}
		case StateTransitionType::Wipe:
		{
			while (m_states.empty() == false)
			{
				PopState();
			}
			PushState(m_pendingTransition.m_toState);
			break;
		}
		default:
		{
			break;
		}
	}

	m_pendingTransition = StateTransition();
}



//----------------------------------------------------------------------------------------------------------------------
bool GameFlow::PushStateEvent(NamedProperties& props)
{
	ASSERT_OR_DIE(m_pendingTransition.m_type == StateTransitionType::None, "GameFlow::ChangeState: There is already a pending state transition.");
	m_pendingTransition.m_type = StateTransitionType::Push;
	m_pendingTransition.m_toState = props.Get<Name>("state", "");
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool GameFlow::PopStateEvent(NamedProperties& props)
{
	ASSERT_OR_DIE(m_pendingTransition.m_type == StateTransitionType::None, "GameFlow::ChangeState: There is already a pending state transition.");
	m_pendingTransition.m_type = StateTransitionType::Pop;
	m_pendingTransition.m_toState = props.Get<Name>("state", Name::Invalid);
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool GameFlow::ChangeStateEvent(NamedProperties& props)
{
	ASSERT_OR_DIE(m_pendingTransition.m_type == StateTransitionType::None, "GameFlow::ChangeState: There is already a pending state transition.");
	bool wipe = props.Get<bool>("wipe", false);
	if (wipe)
	{
		m_pendingTransition.m_type = StateTransitionType::Wipe;
	}
	else
	{
		m_pendingTransition.m_type = StateTransitionType::Change;
	}
	m_pendingTransition.m_toState = props.Get<Name>("state", "");
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool GameFlow::GameFlowDump(NamedProperties&)
{
	for (auto& state : m_states)
	{
		DevConsoleUtils::Log(Rgba8::Yellow, "State: %s", state->m_name.ToCStr());
	}

	return false;
}
