// Bradley Christensen - 2022-2023
#include "Engine.h"
#include "EngineCommon.h"
#include "EngineSubsystem.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Performance/PerformanceDebugWindow.h"
#include "Engine/Core/StringUtils.h"

#include "Game/Framework/EngineBuildPreferences.h"



//----------------------------------------------------------------------------------------------------------------------
// THE ENGINE
//
Engine* g_engine = nullptr;



//----------------------------------------------------------------------------------------------------------------------
void Engine::Startup()
{
    if (m_isActive)
    {
        return;
    }

    for (EngineSubsystem*& subsystem : m_subsystems)
    {
        if (subsystem)
        {
            subsystem->Startup();
        }
    }

    #if defined(PERF_WINDOW_LOG_ENGINE_FRAME_DATA)
        g_performanceDebugWindow->RegisterSection("BeginFrame");
        g_performanceDebugWindow->RegisterSection("Update");
        g_performanceDebugWindow->RegisterSection("Render");
        g_performanceDebugWindow->RegisterSection("EndFrame");
    #endif

    m_isActive = true;
}



//----------------------------------------------------------------------------------------------------------------------
void Engine::BeginFrame()
{
    #if defined(PERF_WINDOW_LOG_ENGINE_FRAME_DATA)
        ScopedTimer beginFrameTimer("BeginFrame");
    #endif

    for (EngineSubsystem*& subsystem : m_subsystems)
    {
        if (subsystem && subsystem->IsEnabled())
        {
            subsystem->BeginFrame();
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Engine::Update(float deltaSeconds)
{
    for (EngineSubsystem*& subsystem : m_subsystems)
    {
        if (subsystem && subsystem->IsEnabled())
        {
            subsystem->Update(deltaSeconds);
        }
    } 
}



//----------------------------------------------------------------------------------------------------------------------
void Engine::Render() const
{
    for (int i = (int) m_subsystems.size() - 1; i >= 0; --i)
    {
        EngineSubsystem* const& subsystem = m_subsystems[i];
        if (subsystem && subsystem->IsEnabled())
        {
            subsystem->Render();
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Engine::EndFrame()
{
    for (int i = (int) m_subsystems.size() - 1; i >= 0; --i)
    {
        EngineSubsystem*& subsystem = m_subsystems[i];
        if (subsystem && subsystem->IsEnabled())
        {
            subsystem->EndFrame();
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Engine::Shutdown()
{
    for (int i = (int) m_subsystems.size() - 1; i >= 0; --i)
    {
        EngineSubsystem*& subsystem = m_subsystems[i];
        if (subsystem)
        {
            SHUTDOWN_AND_DESTROY(subsystem)
        }
    }
    m_subsystems.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void Engine::RegisterSubsystem(EngineSubsystem* system)
{
    if (m_isActive)
    {
        return;
    }

    m_subsystems.push_back(system);
}
