// Bradley Christensen - 2022-2026
#include "Game.h"
#include "GameCommon.h"
#include "GameFlow.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Audio/AudioSystem.h"
#include "Engine/Audio/AudioUtils.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Multithreading/JobSystem.h"
#include "Engine/Performance/PerformanceDebugWindow.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Window/Window.h"
#include "Engine/Window/WindowUtils.h"
#include "Game/Framework/Application.h"



//----------------------------------------------------------------------------------------------------------------------
Game::Game() : EngineSubsystem("Game")
{
    ASSERT_OR_DIE(g_engine != nullptr, "Null engine when constructing game");
}



//----------------------------------------------------------------------------------------------------------------------
Game::~Game()
{

}



//----------------------------------------------------------------------------------------------------------------------
void Game::Startup()
{
	g_eventSystem->SubscribeMethod("Quit", this, &Game::QuitEvent);

	m_gameFlow = new GameFlow();
	m_gameFlow->Startup();
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	g_eventSystem->UnsubscribeMethod("Quit", this, &Game::QuitEvent);

    m_gameFlow->Shutdown();
    delete m_gameFlow;
    m_gameFlow = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Game::BeginFrame()
{
	m_gameFlow->BeginFrame();
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Update(float deltaSeconds)
{
	m_gameFlow->Update(deltaSeconds);
}



//----------------------------------------------------------------------------------------------------------------------
void Game::EndFrame()
{
	m_gameFlow->EndFrame();
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Render() const
{
	m_gameFlow->Render();
}



//----------------------------------------------------------------------------------------------------------------------
void Game::ConfigureEngine(Engine* engine)
{
    JobSystemConfig jobSysConfig;
    jobSysConfig.m_threadCount = std::thread::hardware_concurrency();
    g_jobSystem = new JobSystem(jobSysConfig);
    engine->RegisterSubsystem(g_jobSystem);

	AssetManagerConfig assetManagerConfig;
	g_assetManager = new AssetManager(assetManagerConfig);
	engine->RegisterSubsystem(g_assetManager);

    EventSystemConfig eventSysConfig;
    g_eventSystem = new EventSystem(eventSysConfig);
    engine->RegisterSubsystem(g_eventSystem);

    AudioSystemConfig audioConfig;
    g_audioSystem = AudioUtils::MakeAudioSystem(audioConfig);
    engine->RegisterSubsystem(g_audioSystem);

    RendererConfig rendererConfig;
	rendererConfig.m_startupUserSettings.m_vsyncEnabled = false;
	rendererConfig.m_startupUserSettings.m_msaaEnabled = true;
    g_renderer = RendererUtils::MakeRenderer(rendererConfig);
    engine->RegisterSubsystem(g_renderer);

    WindowConfig windowConfig;
    windowConfig.m_windowTitle = "Project Daley";
    windowConfig.m_startupUserSettings.m_windowMode = WindowMode::Borderless;
    windowConfig.m_startupUserSettings.m_windowedResolution = IntVec2(500, 500);
	g_window = WindowUtils::MakeWindow(windowConfig);
    engine->RegisterSubsystem(g_window);

    // Dev console before input, so it steals input from the window when active
    DevConsoleConfig dcConfig;
	dcConfig.m_backgroundTint = Rgba8(0, 0, 0, 100);
    dcConfig.m_backgroundImageSustainSeconds = 30.f;
    dcConfig.m_backgroundImageFadeSeconds = 1.f;
    dcConfig.m_openCloseAnimationSpeed = 10.f;
    dcConfig.m_backgroundImages = { "IceCream.png" };
    dcConfig.m_openSoundFilePath = "Data/Sounds/SFX/WaterDroplet1.wav";
    g_devConsole = new DevConsole(dcConfig);
    engine->RegisterSubsystem(g_devConsole);

    InputSystemConfig inputConfig;
    g_input = new InputSystem(inputConfig);
    engine->RegisterSubsystem(g_input);

    PerformanceDebugWindowConfig perfDebugWindowConfig;
    g_performanceDebugWindow = new PerformanceDebugWindow(perfDebugWindowConfig);
    engine->RegisterSubsystem(g_performanceDebugWindow);
}



//----------------------------------------------------------------------------------------------------------------------
bool Game::Quit()
{
    g_app->Quit();
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool Game::QuitEvent(NamedProperties&)
{
	g_app->Quit();
	return true;
}
