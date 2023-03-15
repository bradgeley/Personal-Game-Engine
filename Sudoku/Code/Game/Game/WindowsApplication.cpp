// Bradley Christensen - 2022-2023
#include "WindowsApplication.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/Time.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Window.h"
#include "Game.h"



//----------------------------------------------------------------------------------------------------------------------
// THE APP
//
WindowsApplication* g_app = nullptr;



void WindowsApplication::Startup()
{
    m_engine = new Engine();
    
    WindowConfig windowConfig;
    windowConfig.m_windowTitle = "Sudoku";
    windowConfig.m_clientAspect = 1.f;
    windowConfig.m_windowScale = 0.5;
    g_window = new Window(windowConfig);
    m_engine->RegisterSubsystem(g_window);

    g_window->m_quit.SubscribeMethod(this, &WindowsApplication::HandleQuit);

    RendererConfig rendererConfig;
    g_renderer = new Renderer(rendererConfig);
    m_engine->RegisterSubsystem(g_renderer);

    InputSystemConfig inputConfig;
    g_input = new InputSystem(inputConfig);
    m_engine->RegisterSubsystem(g_input);
    
    m_engine->Startup();

    m_game = new Game();
    m_game->Startup();
}



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
        m_engine->Update(deltaSeconds);
        m_game->Update(deltaSeconds);
        m_game->Render();
        m_engine->Render();
        m_engine->EndFrame();
    }
}



void WindowsApplication::Shutdown()
{
    SHUTDOWN_AND_DESTROY(m_game)
    SHUTDOWN_AND_DESTROY(m_engine)
}



void WindowsApplication::Quit()
{
    m_isQuitting = true;
}



bool WindowsApplication::HandleQuit(NamedProperties& args)
{
    UNUSED(args)
    Quit();
    return true;
}



bool WindowsApplication::IsQuitting() const
{
    return m_isQuitting;
}
