// Bradley Christensen - 2022-2025
#include "Engine.h"
#include "EngineCommon.h"
#include "EngineSubsystem.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Performance/PerformanceDebugWindow.h"
#include "Engine/Time/Clock.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/NameTable.h"

#include "Game/Framework/EngineBuildPreferences.h"

#if defined(PERF_WINDOW_LOG_ENGINE_FRAME_DATA)
#include "Engine/Time/Time.h"
#include "Engine/Renderer/Renderer.h"
#endif



//----------------------------------------------------------------------------------------------------------------------
// THE Engine
//
Engine* g_engine = nullptr;



//----------------------------------------------------------------------------------------------------------------------
#if defined(PERF_WINDOW_LOG_ENGINE_FRAME_DATA)
static PerfFrameData s_frameData;
#endif



//----------------------------------------------------------------------------------------------------------------------
Engine::Engine()
{
    g_nameTable = new NameTable();
    g_nameTable->Startup();
    m_engineClock = new Clock();
}



//----------------------------------------------------------------------------------------------------------------------
Engine::~Engine()
{
    delete m_engineClock;
    m_engineClock = nullptr;
    g_nameTable->Shutdown();
    delete g_nameTable;
    g_nameTable = nullptr;
}



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

    m_isActive = true;
}



//----------------------------------------------------------------------------------------------------------------------
void Engine::BeginFrame()
{
    m_engineClock->Update();

    #if defined(PERF_WINDOW_LOG_ENGINE_FRAME_DATA)
        s_frameData.m_engineFrameStartTime = m_engineClock->GetCurrentTimeSeconds();
    #endif

    #if defined(PERF_WINDOW_DISPLAY_ENGINE_SECTION)
        PerfWindowScopedTimer scopedTimer("Engine","BeginFrame");
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
void Engine::Update()
{
    double deltaSeconds = m_engineClock->GetDeltaSeconds();

    #if defined(PERF_WINDOW_LOG_ENGINE_FRAME_DATA)
        s_frameData.m_actualDeltaSeconds = deltaSeconds;
    #endif

    #if defined(PERF_WINDOW_DISPLAY_ENGINE_SECTION)
        PerfWindowScopedTimer scopedTimer("Engine", "Update");
    #endif

    for (EngineSubsystem*& subsystem : m_subsystems)
    {
        if (subsystem && subsystem->IsEnabled())
        {
            subsystem->Update(static_cast<float>(deltaSeconds));
        }
    } 
}



//----------------------------------------------------------------------------------------------------------------------
void Engine::Render() const
{
    #if defined(PERF_WINDOW_DISPLAY_ENGINE_SECTION)
        PerfWindowScopedTimer scopedTimer("Engine", "Render");
    #endif

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
    #if defined(PERF_WINDOW_DISPLAY_ENGINE_SECTION)
        PerfWindowScopedTimer scopedTimer("Engine", "EndFrame");
    #endif

    for (int i = (int) m_subsystems.size() - 1; i >= 0; --i)
    {
        EngineSubsystem*& subsystem = m_subsystems[i];
        if (subsystem && subsystem->IsEnabled())
        {
            subsystem->EndFrame();
        }
    }

    #if defined(PERF_WINDOW_LOG_ENGINE_FRAME_DATA)
        s_frameData.m_engineFrameEndTime = GetCurrentTimeSeconds();

        if (g_performanceDebugWindow)
        {
            g_performanceDebugWindow->UpdateFrameData(s_frameData);
            g_performanceDebugWindow->EngineFrameCompleted();
        }
    #endif
}



//----------------------------------------------------------------------------------------------------------------------
void Engine::Shutdown()
{
    for (int i = (int) m_subsystems.size() - 1; i >= 0; --i)
    {
        EngineSubsystem*& subsystem = m_subsystems[i];
        if (subsystem)
        {
            subsystem->Shutdown();
            delete subsystem;
            subsystem = nullptr;
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



//----------------------------------------------------------------------------------------------------------------------
Clock* Engine::GetEngineClock() const
{
    return m_engineClock;
}
