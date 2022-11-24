// Bradley Christensen - 2022
#include "WindowsApplication.h"
#include "Engine/Core/Engine.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
// THE App
//
WindowsApplication* g_theApp = nullptr;



WindowsApplication::WindowsApplication()
{
    
}



void WindowsApplication::Startup()
{
    WindowConfig windowConfig;
    g_window = new Window(windowConfig);
    g_engine->RegisterSubsystem(g_window);
}



void WindowsApplication::Run()
{
    float deltaSeconds = 0.f; // todo: calculate
    while (!m_isQuitting)
    {
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
    m_engine->Shutdown();
}



void WindowsApplication::Quit()
{
    m_isQuitting = true;
}


bool WindowsApplication::IsQuitting() const
{
    return m_isQuitting;
}
