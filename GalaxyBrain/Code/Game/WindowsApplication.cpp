// Bradley Christensen - 2022-2023
#include "WindowsApplication.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/Time.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Multithreading/JobSystem.h"
#include "Engine/Multithreading/JobSystemDebug.h"
#include "Game.h"



//----------------------------------------------------------------------------------------------------------------------
// THE APP
//
WindowsApplication* g_app = nullptr;



//----------------------------------------------------------------------------------------------------------------------
void WindowsApplication::Startup()
{
    m_engine = new Engine();

    g_rng = new RandomNumberGenerator();

    EventSystemConfig eventSysConfig;
    g_eventSystem = new EventSystem(eventSysConfig);
    m_engine->RegisterSubsystem(g_eventSystem);
    
    WindowConfig windowConfig;
    windowConfig.m_windowTitle = "Galaxy Brain";
    windowConfig.m_clientAspect = 2.f;
    windowConfig.m_windowScale = 0.95f;
    g_window = new Window(windowConfig);
    m_engine->RegisterSubsystem(g_window);

    g_window->m_quit.SubscribeMethod(this, &WindowsApplication::HandleQuit);

    RendererConfig rendererConfig;
    g_renderer = new Renderer(rendererConfig);
    m_engine->RegisterSubsystem(g_renderer);

    // Dev console before input, so it steals input from the window when active
    DevConsoleConfig dcConfig;
    dcConfig.m_backgroundImages = { "DrStrange.jpg" , "Thanos.jpg", "IronMan.jpg", "Thor.jpg" };
    g_devConsole = new DevConsole(dcConfig);
    m_engine->RegisterSubsystem(g_devConsole);
    
    InputSystemConfig inputConfig;
    g_input = new InputSystem(inputConfig);
    m_engine->RegisterSubsystem(g_input);

    JobSystemConfig jobSysConfig;
    g_jobSystem = new JobSystem(jobSysConfig);
    m_engine->RegisterSubsystem(g_jobSystem);
    
    m_engine->Startup();

    m_game = new Game();
    m_game->Startup();
}



//----------------------------------------------------------------------------------------------------------------------
void WindowsApplication::Run() 
{
    static float timeThen = GetCurrentTimeSecondsF();
    
    while (!IsQuitting())
    {
        // todo: move to system clock and world clock, etc
        float timeNow = GetCurrentTimeSecondsF();
        float deltaSeconds = timeNow - timeThen;
        timeThen = timeNow;
        //
        
        m_engine->BeginFrame();
        m_game->BeginFrame();
        
        m_engine->Update(deltaSeconds);
        m_game->Update(deltaSeconds);
        
        m_game->Render();
        m_engine->Render();
        
        m_game->EndFrame();
        m_engine->EndFrame();
    }
}



//----------------------------------------------------------------------------------------------------------------------
void WindowsApplication::Shutdown()
{
    SHUTDOWN_AND_DESTROY(m_game)
    SHUTDOWN_AND_DESTROY(m_engine)
    delete g_rng;
    g_rng = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void WindowsApplication::Quit()
{
    m_isQuitting = true;
}



//----------------------------------------------------------------------------------------------------------------------
bool WindowsApplication::HandleQuit(NamedProperties& args)
{
    UNUSED(args)
    Quit();
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool WindowsApplication::IsQuitting() const
{
    return m_isQuitting;
}
