// Bradley Christensen - 2022
#include "Game/WindowsApplication.h"
#include "Engine/Core/Engine.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
// THE APP
//
WindowsApplication* g_theApp = nullptr;



void WindowsApplication::Startup()
{
    m_engine = new Engine();
    
    WindowConfig windowConfig;
    g_window = new Window(windowConfig);
    //g_engine->RegisterSubsystem(g_window);

    m_engine->Startup();
}



void WindowsApplication::Run()
{
    float deltaSeconds = 0.f; // todo: calculate
    //while (!m_isQuitting)
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
    m_engine->Shutdown();
    delete m_engine;
}



void WindowsApplication::Quit()
{
    m_isQuitting = true;
}


bool WindowsApplication::IsQuitting() const
{
    return m_isQuitting;
}
