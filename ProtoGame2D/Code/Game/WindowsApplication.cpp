// Bradley Christensen - 2022-2023
#include "Game/WindowsApplication.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/Time.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Window.h"
#include "Game/Game.h"



//----------------------------------------------------------------------------------------------------------------------
// THE APP
//
WindowsApplication* g_theApp = nullptr;



void WindowsApplication::Startup()
{
    m_engine = new Engine();
    
    WindowConfig windowConfig;
    windowConfig.m_windowTitle = "Protogame";
    windowConfig.m_clientAspect = 2.f;
    windowConfig.m_windowScale = 0.5;
    g_window = new Window(windowConfig);
    m_engine->RegisterSubsystem(g_window);

    g_window->m_quit.SubscribeMethod(*this, &WindowsApplication::HandleQuit);

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
    m_game->Shutdown();
    delete m_game;
    m_game = nullptr;
    
    m_engine->Shutdown();
    delete m_engine;
    m_engine = nullptr;
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
