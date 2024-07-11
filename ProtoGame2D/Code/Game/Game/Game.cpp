// Bradley Christensen - 2022-2023
#include "Game.h"
#include "GameCommon.h"
#include "WindowsApplication.h"
#include "Engine/Core/Engine.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Multithreading/JobSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void Game::Startup()
{
    g_rng = new RandomNumberGenerator();
}



//----------------------------------------------------------------------------------------------------------------------
void Game::BeginFrame()
{

}



//----------------------------------------------------------------------------------------------------------------------
void Game::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
    if (g_input->WasKeyJustReleased(KeyCode::Escape))
    {
        g_app->Quit();
    }

    
}



//----------------------------------------------------------------------------------------------------------------------
void Game::EndFrame()
{

}



//----------------------------------------------------------------------------------------------------------------------
void Game::Render() const
{
    Camera dummyCamera = Camera(0.f, 0.f, 1.f, 1.f);
    g_renderer->BeginCamera(&dummyCamera);
    g_renderer->ClearScreen(Rgba8::White);
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Shutdown()
{
    delete g_rng;
    g_rng = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Game::ConfigureEngine(Engine* engine) const
{
    EventSystemConfig eventSysConfig;
    g_eventSystem = new EventSystem(eventSysConfig);
    engine->RegisterSubsystem(g_eventSystem);

    WindowConfig windowConfig;
    windowConfig.m_windowTitle = "Protogame 2D";
    windowConfig.m_clientAspect = 2.f;
    windowConfig.m_windowScale = 0.95f;
    g_window = new Window(windowConfig);
    engine->RegisterSubsystem(g_window);

    RendererConfig rendererConfig;
    g_renderer = new Renderer(rendererConfig);
    engine->RegisterSubsystem(g_renderer);

    // Dev console before input, so it steals input from the window when active
    DevConsoleConfig dcConfig;
    dcConfig.m_backgroundImageSustainSeconds = 30.f;
    dcConfig.m_backgroundImageFadeSeconds = 1.f;
    dcConfig.m_backgroundImages = { "DrStrange.jpg" , "Hawkeye.jpg", "Thanos.jpg", "Avengers.png", "IronMan.jpg", "Jack1.jpg", "Jack2.jpg", "Thor.jpg" };
    g_devConsole = new DevConsole(dcConfig);
    engine->RegisterSubsystem(g_devConsole);

    InputSystemConfig inputConfig;
    g_input = new InputSystem(inputConfig);
    engine->RegisterSubsystem(g_input);

    JobSystemConfig jobSysConfig;
    g_jobSystem = new JobSystem(jobSysConfig);
    engine->RegisterSubsystem(g_jobSystem);
}
