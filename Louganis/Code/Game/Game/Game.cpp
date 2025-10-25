// Bradley Christensen - 2022-2025
#include "Game.h"

#include "Engine/Audio/AudioSystem.h"
#include "Engine/Audio/AudioUtils.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/Image.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/TextureAsset.h"
#include "Engine/Time/Clock.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Multithreading/JobSystem.h"
#include "Engine/Performance/PerformanceDebugWindow.h"
#include "Engine/Window/Window.h"
#include "Engine/Window/WindowUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Font.h"

#include "Game/Framework/Application.h"
#include "AllComponents.h"
#include "AllSystems.h"
#include "EntityDef.h"
#include "GameCommon.h"
#include "TileDef.h"



//----------------------------------------------------------------------------------------------------------------------
std::vector<std::string> g_framePhaseNames = { "PrePhysics", "Physics", "PostPhysics", "Render", "PostRender"}; // todo: write enum to string macro?

enum class FramePhase : int
{
    PrePhysics,
    Physics,
    PostPhysics,
    Render,
    PostRender,
};



//----------------------------------------------------------------------------------------------------------------------
Game::Game() : EngineSubsystem("Game")
{
    ASSERT_OR_DIE(g_engine != nullptr, "Null engine when constructing game");
    m_gameClock = new Clock(g_engine->GetEngineClock());
}



//----------------------------------------------------------------------------------------------------------------------
Game::~Game()
{
    delete m_gameClock;
    m_gameClock = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Startup()
{
    ConfigureECS();
    g_ecs->Startup();

    g_devConsole->AddDevConsoleCommandInfo("TimeDilation", "t", DevConsoleArgType::Float);
    g_eventSystem->SubscribeMethod("TimeDilation", this, &Game::TimeDilation);
}



//----------------------------------------------------------------------------------------------------------------------
void Game::BeginFrame()
{

}



//----------------------------------------------------------------------------------------------------------------------
void Game::Update(float)
{
    if (g_input->WasKeyJustReleased(KeyCode::Escape))
    {
        g_app->Quit();
    }

    g_ecs->RunFrame(m_gameClock->GetDeltaSecondsF());
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
    g_eventSystem->UnsubscribeMethod("TimeDilation", this, &Game::TimeDilation);
    g_devConsole->RemoveDevConsoleCommandInfo("TimeDilation");

    g_ecs->Shutdown();

    delete g_ecs;
    g_ecs = nullptr;
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
	rendererConfig.m_startupUserSettings.m_msaaEnabled = false;
    g_renderer = RendererUtils::MakeRenderer(rendererConfig);
    engine->RegisterSubsystem(g_renderer);

    WindowConfig windowConfig;
    windowConfig.m_windowTitle = "Project Louganis";
    windowConfig.m_startupUserSettings.m_windowMode = WindowMode::Borderless;
    windowConfig.m_startupUserSettings.m_windowedResolution = IntVec2(500, 500);
	g_window = WindowUtils::MakeWindow(windowConfig);
    engine->RegisterSubsystem(g_window);

    // Dev console before input, so it steals input from the window when active
    DevConsoleConfig dcConfig;
    dcConfig.m_backgroundImageSustainSeconds = 30.f;
    dcConfig.m_backgroundImageFadeSeconds = 1.f;
    dcConfig.m_openCloseAnimationSpeed = 10.f;
    dcConfig.m_backgroundImages = { "DrStrange.jpg" , "Hawkeye.jpg", "Thanos.jpg", "Avengers.png", "IronMan.jpg", "Jack1.jpg", "Jack2.jpg", "Thor.jpg" };
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
void Game::ConfigureECS()
{
    AdminSystemConfig ecsConfig;
    ecsConfig.m_maxDeltaSeconds = 0.1f;
    ecsConfig.m_enableMultithreading = true;
    ecsConfig.m_autoMultithreadingEntityThreshold = 1;
    ecsConfig.m_systemSplittingEntityThreshold = 1;
    g_ecs = new AdminSystem(ecsConfig);

    //----------------------------------------------------------------------------------------------------------------------
    // COMPONENTS
    // 

    // Array components
    g_ecs->RegisterComponentArray<CAbility>();
    g_ecs->RegisterComponentArray<CAIController>();
    g_ecs->RegisterComponentArray<CAnimation>();
    g_ecs->RegisterComponentArray<CCollision>();
    g_ecs->RegisterComponentArray<CDeath>();
    g_ecs->RegisterComponentArray<CHealth>();
    g_ecs->RegisterComponentArray<CLifetime>();
    g_ecs->RegisterComponentArray<CMovement>();
    g_ecs->RegisterComponentArray<CRender>();
    g_ecs->RegisterComponentArray<CTime>();
    g_ecs->RegisterComponentArray<CTransform>();

    // Map components
    g_ecs->RegisterComponentMap<CPlayerController>();

    // Singleton components
    g_ecs->RegisterComponentSingleton<SCAudio>();
    g_ecs->RegisterComponentSingleton<SCCamera>();
    g_ecs->RegisterComponentSingleton<SCCollision>();
    g_ecs->RegisterComponentSingleton<SCDebug>();
    g_ecs->RegisterComponentSingleton<SCEntityFactory>();
    g_ecs->RegisterComponentSingleton<SCFlowField>();
    g_ecs->RegisterComponentSingleton<SCLighting>();
    g_ecs->RegisterComponentSingleton<SCLoadChunks>();
    g_ecs->RegisterComponentSingleton<SCRender>();
    g_ecs->RegisterComponentSingleton<SCStorm>();
    g_ecs->RegisterComponentSingleton<SCTime>();
    g_ecs->RegisterComponentSingleton<SCWorld>();

    // Other resource types (Engine Subsystems)
    g_ecs->RegisterResourceByType<AudioSystem>();
    g_ecs->RegisterResourceByType<InputSystem>();
    g_ecs->RegisterResourceByType<Renderer>();
    g_ecs->RegisterResourceByType<AssetManager>();


    //----------------------------------------------------------------------------------------------------------------------
    // SYSTEMS
    // 

    // Pre Physics
    g_ecs->RegisterSystem<STime>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SLifetime>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SEntityFactory>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SInput>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SDebugInput>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SAbility>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SStorm>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SWorld>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SLoadChunks>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SUnloadChunks>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SBackgroundMusic>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SFlowField>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SAIController>((int) FramePhase::PrePhysics);

    // Physics
    //SystemSubgraph& physics = g_ecs->CreateOrGetSystemSubgraph((int) FramePhase::Physics);
    //physics.m_timeStep = 0.01f;
    g_ecs->RegisterSystem<SMovement>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SPhysics>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SCollisionHash>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SCollision>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SWorldCollision>((int) FramePhase::Physics);

    // Post-Physics
    g_ecs->RegisterSystem<SHealth>((int)FramePhase::PostPhysics);

    // Render
    g_ecs->RegisterSystem<SCopyTransform>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SCamera>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SInitView>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SMovementAnimation>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SAnimation>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SLighting>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderWorld>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderEntities>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderStorm>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderUI>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SDebugRender>((int) FramePhase::Render);
	g_ecs->RegisterSystem<SDebugOverlay>((int) FramePhase::Render);

    // debug
    g_ecs->RegisterSystem<SDebugCommands>((int) FramePhase::PostRender);
    g_ecs->RegisterSystem<SSystemDebug>((int) FramePhase::PostRender);
    g_ecs->RegisterSystem<SDeath>((int) FramePhase::PostRender);
}



//----------------------------------------------------------------------------------------------------------------------
bool Game::TimeDilation(NamedProperties& args)
{
    float currentTimeDilation = m_gameClock->GetLocalTimeDilationF();
    float timeDilation = 1.f;
    timeDilation = args.Get("t", timeDilation);
    if (currentTimeDilation != timeDilation)
    {
        m_gameClock->SetLocalTimeDilation(timeDilation);
        g_devConsole->LogSuccess("time dilation successfully changed");
    }
    else
    {
        g_devConsole->LogWarning("time dilation unchanged");
    }
    return false;
}
