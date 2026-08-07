// Bradley Christensen - 2022-2026
#include "GameFlow.h"
#include "MainMenu.h"
#include "TowerDefense.h"
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

	PushState("MainMenu");
}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::Shutdown()
{
	g_eventSystem->UnsubscribeMethod("PushState", this, &GameFlow::PushStateEvent);
	g_eventSystem->UnsubscribeMethod("PopState", this, &GameFlow::PopStateEvent);
	g_eventSystem->UnsubscribeMethod("ChangeState", this, &GameFlow::ChangeStateEvent);

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
	for (auto& state : m_states)
	{
		state->Update(deltaSeconds);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void GameFlow::Render() const
{
	for (auto& state : m_states)
	{
		state->Render();
	}
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
		m_states.push_back(new MainMenu());
	}
	else if (state == "TowerDefense")
	{
		m_states.push_back(new TowerDefense());
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
	m_pendingTransition.m_type = StateTransitionType::Change;
	m_pendingTransition.m_toState = props.Get<Name>("state", "");
	return false;
}