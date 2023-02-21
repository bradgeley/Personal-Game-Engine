// Bradley Christensen - 2022
#include "Game/WindowsApplication.h"
#include "Engine/Core/Engine.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
// THE APP
//
WindowsApplication* g_theApp = nullptr;



void WindowsApplication::Startup()
{
    m_engine = new Engine();
    
    WindowConfig windowConfig;
    windowConfig.m_windowTitle = "Protogame";
    windowConfig.m_clientAspect = 1.f;
    windowConfig.m_windowScale = 0.5;
    g_window = new Window(windowConfig);
    m_engine->RegisterSubsystem(g_window);

    RendererConfig rendererConfig;
    g_renderer = new Renderer(rendererConfig);
    m_engine->RegisterSubsystem(g_renderer);
    
    m_engine->Startup();
}



void WindowsApplication::Run()
{
    float deltaSeconds = 0.f; // todo: calculate
    while (!IsQuitting())
    {
        m_engine->BeginFrame();
        m_engine->Update(deltaSeconds);
        // m_game->Update(deltaSeconds);
        // m_game->Render();
        m_engine->Render();
        m_engine->EndFrame();
    }
}



void WindowsApplication::Shutdown()
{
    //m_game->Shutdown();
    //delete m_game;
    //m_game = nullptr;
    m_engine->Shutdown();
    delete m_engine;
    m_engine = nullptr;
}



void WindowsApplication::Quit()
{
    g_window->m_isQuitting = true;
}



bool WindowsApplication::IsQuitting() const
{
    return g_window->m_isQuitting;
}
