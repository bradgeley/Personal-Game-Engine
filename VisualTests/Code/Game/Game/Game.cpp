// Bradley Christensen - 2022-2023
#include "Game.h"
#include "GameCommon.h"
#include "WindowsApplication.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Multithreading/JobSystem.h"
#include "Engine/Math/MathUtils.h"
#include "Game/Game/NoiseTest.h"
#include "Game/Game/Noise2DTest.h"
#include "Game/Game/Disc2DCollisionTest.h"
#include "Game/Game/FormationMovementTest.h"



//----------------------------------------------------------------------------------------------------------------------
void Game::Startup()
{
    g_rng = new RandomNumberGenerator();

    m_testModes.push_back(new FormationMovementTest());
    m_testModes.push_back(new NoiseTest());
    m_testModes.push_back(new Noise2DTest());
    m_testModes.push_back(new Disc2DCollisionTest());

    m_currentTestMode = 0;
    m_testModes[m_currentTestMode]->Startup();

    g_devConsole->m_helpDelegate.SubscribeMethod(this, &Game::Help);
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

    if (g_input->WasKeyJustPressed(KeyCode::Right))
    {
        m_testModes[m_currentTestMode]->Shutdown();
        m_currentTestMode = IncrementIntInRange(m_currentTestMode, 0, (int) m_testModes.size() - 1, true);
        m_testModes[m_currentTestMode]->Startup();
    }
    else if (g_input->WasKeyJustPressed(KeyCode::Left))
    {
        m_testModes[m_currentTestMode]->Shutdown();
        m_currentTestMode = DecrementIntInRange(m_currentTestMode, 0, (int) m_testModes.size() - 1, true);
        m_testModes[m_currentTestMode]->Startup();
    }
    else
    {
        // Run test frame
        m_testModes[m_currentTestMode]->BeginFrame();
        m_testModes[m_currentTestMode]->Update(deltaSeconds);
        m_testModes[m_currentTestMode]->Render();
        m_testModes[m_currentTestMode]->EndFrame();
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Game::EndFrame()
{
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Render() const
{
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Shutdown()
{
    delete g_rng;
    g_rng = nullptr;

    m_testModes[m_currentTestMode]->Shutdown();
    for (auto& mode : m_testModes)
    {
        delete mode;
        mode = nullptr;
    }
    m_testModes.clear();
}



//----------------------------------------------------------------------------------------------------------------------
bool Game::Help(NamedProperties& args)
{
    UNUSED(args)
    g_devConsole->LogWarning("Change test modes: Right/Left arrow");
    m_testModes[m_currentTestMode]->DisplayHelpMessage();
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
void Game::ConfigureEngine(Engine* engine) const
{
    EventSystemConfig eventSysConfig;
    g_eventSystem = new EventSystem(eventSysConfig);
    engine->RegisterSubsystem(g_eventSystem);

    WindowConfig windowConfig;
    windowConfig.m_windowTitle = "Visual Tests";
    windowConfig.m_clientAspect = 2.f;
    windowConfig.m_windowScale = 0.95f;
    g_window = new Window(windowConfig);
    engine->RegisterSubsystem(g_window);

    RendererConfig rendererConfig;
    g_renderer = new Renderer(rendererConfig);
    engine->RegisterSubsystem(g_renderer);

    // Dev console before input, so it steals input from the window when active
    DevConsoleConfig dcConfig;
    dcConfig.m_backgroundImageSustainSeconds = 1.f;
    dcConfig.m_backgroundImageFadeSeconds = 1.f;
    dcConfig.m_backgroundImages = { "DrStrange.jpg", "Thanos.jpg", "Avengers.png", "IronMan.jpg", "Thor.jpg" };
    g_devConsole = new DevConsole(dcConfig);
    engine->RegisterSubsystem(g_devConsole);

    InputSystemConfig inputConfig;
    g_input = new InputSystem(inputConfig);
    engine->RegisterSubsystem(g_input);

    JobSystemConfig jobSysConfig;
    g_jobSystem = new JobSystem(jobSysConfig);
    engine->RegisterSubsystem(g_jobSystem);
}
