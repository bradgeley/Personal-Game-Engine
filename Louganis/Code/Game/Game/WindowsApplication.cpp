// Bradley Christensen - 2022-2023
#include "WindowsApplication.h"
#include "Engine/Core/Engine.h"
#include "Game.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/Time.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
// THE APP
//
WindowsApplication* g_app = nullptr;



void WindowsApplication::Startup()
{
    g_engine = new Engine();
    m_game = new Game();

    m_game->ConfigureEngine(g_engine); // Adds subsystems to engine that the game uses

    g_engine->RegisterSubsystem(m_game); // Add the game itself as the final subsystem

    g_engine->Startup(); // Start up the configured Engine with the game registered as the final subsystem

    g_window->m_quit.SubscribeMethod(this, &WindowsApplication::HandleQuit);
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

        g_engine->BeginFrame();
        g_engine->Update(deltaSeconds);
        g_engine->Render();
        g_engine->EndFrame();
    }
}



void WindowsApplication::Shutdown()
{
    SHUTDOWN_AND_DESTROY(g_engine)
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
