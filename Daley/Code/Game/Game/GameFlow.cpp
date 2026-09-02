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

	NamedProperties props;
	PushState("MainMenu", props);
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
		NamedProperties props;
		PopState(props);
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
void GameFlow::PushState(Name state, NamedProperties const& props)
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

	m_states.back()->Enter(props);
}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::PopState(NamedProperties const& props)
{
	ASSERT_OR_DIE(m_states.empty() == false, "Cannot pop state when no states are present.");

	GameState* top = m_states.back();
	m_states.pop_back();

	DevConsoleUtils::Log(Rgba8::Yellow, "Exiting state: %s", top->m_name.ToCStr());

	top->Exit(props);
	delete top;
}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::PopToState(Name state, NamedProperties const& props)
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
		PopState(props);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::HandlePendingTransition()
{
	while (m_pendingTransitions.empty() == false)
	{
		StateTransition const& transition = m_pendingTransitions.front();
		Name toState = transition.m_props.Get<Name>("state", Name::Invalid);

		switch (transition.m_type)
		{
			case StateTransitionType::Push:
			{
				PushState(toState, transition.m_props);
				break;
			}
			case StateTransitionType::Pop:
			{
				if (toState == Name::Invalid)
				{
					PopState(transition.m_props);
				}
				else
				{
					PopToState(toState, transition.m_props);
				}
				break;
			}
			case StateTransitionType::Change:
			{
				PopState(transition.m_props);
				PushState(toState, transition.m_props);
				break;
			}
			case StateTransitionType::Wipe:
			{
				while (m_states.empty() == false)
				{
					PopState(transition.m_props);
				}
				PushState(toState, transition.m_props);
				break;
			}
			default:
			{
				break;
			}
		}

		m_pendingTransitions.erase(m_pendingTransitions.begin());
	}
	
}



//----------------------------------------------------------------------------------------------------------------------
bool GameFlow::PushStateEvent(NamedProperties& props)
{
	m_pendingTransitions.push_back(StateTransition(StateTransitionType::Push, props));
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool GameFlow::PopStateEvent(NamedProperties& props)
{
	m_pendingTransitions.push_back(StateTransition(StateTransitionType::Pop, props));
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool GameFlow::ChangeStateEvent(NamedProperties& props)
{
	bool wipe = props.Get<bool>("wipe", false);
	if (wipe)
	{
		m_pendingTransitions.push_back(StateTransition(StateTransitionType::Wipe, props));
	}
	else
	{
		m_pendingTransitions.push_back(StateTransition(StateTransitionType::Change, props));
	}
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



//----------------------------------------------------------------------------------------------------------------------
StateTransition::StateTransition(StateTransitionType type, NamedProperties const& props) : m_type(type)
{
	m_props = props;
}
