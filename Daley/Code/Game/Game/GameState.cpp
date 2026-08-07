// Bradley Christensen - 2022-2026
#include "GameState.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Time/Clock.h"



//----------------------------------------------------------------------------------------------------------------------
void GameState::Enter()
{
    m_clock = new Clock(g_engine->GetEngineClock());

    DevConsoleUtils::AddCommandInfo("TimeDilation", "t", DevConsoleArgType::Float);
    g_eventSystem->SubscribeMethod("TimeDilation", this, &GameState::TimeDilation);
    g_eventSystem->SubscribeMethod("TogglePaused", this, &GameState::TogglePausedEvent);
}



//----------------------------------------------------------------------------------------------------------------------
void GameState::Exit()
{
    g_devConsole->RemoveDevConsoleCommandInfo("TimeDilation");
    g_eventSystem->UnsubscribeMethod("TimeDilation", this, &GameState::TimeDilation);
    g_eventSystem->UnsubscribeMethod("TogglePaused", this, &GameState::TogglePausedEvent);

    delete m_clock;
    m_clock = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
bool GameState::IsPaused() const
{
    return m_clock->IsPaused();
}



//----------------------------------------------------------------------------------------------------------------------
void GameState::TogglePaused()
{
    m_clock->TogglePaused();
}



//----------------------------------------------------------------------------------------------------------------------
void GameState::Pop()
{
    NamedProperties props;
	g_eventSystem->FireEvent("PopState", props);
}



//----------------------------------------------------------------------------------------------------------------------
void GameState::RequestStateChange(Name toState)
{
    NamedProperties props;
    props.Set<Name>("state", toState);
    g_eventSystem->FireEvent("ChangeState", props);
}



//----------------------------------------------------------------------------------------------------------------------
bool GameState::TimeDilation(NamedProperties& args)
{
    float currentTimeDilation = m_clock->GetLocalTimeDilationF();
    float timeDilation = 1.f;
    timeDilation = args.Get("t", timeDilation);
    if (currentTimeDilation != timeDilation)
    {
        m_clock->SetLocalTimeDilation(timeDilation);
        g_devConsole->LogSuccess("time dilation successfully changed");
    }
    else
    {
        g_devConsole->LogWarning("time dilation unchanged");
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool GameState::TogglePausedEvent(NamedProperties&)
{
    m_clock->TogglePaused();
    return false;
}

