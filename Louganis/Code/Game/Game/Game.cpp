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
#include "Engine/Multithreading/JobSystemDebug.h"
#include "Engine/ECS/AdminSystem.h"
#include "EntityDef.h"
#include "TileDef.h"
#include "AllComponents.h"
#include "AllSystems.h"



//----------------------------------------------------------------------------------------------------------------------
std::vector<std::string> g_framePhaseNames = { "PrePhysics", "Physics", "PostPhysics", "Render" }; // todo: write enum to string macro?

enum class FramePhase : int
{
    PrePhysics,
    Physics,
    PostPhysics,
    Render,
};



//----------------------------------------------------------------------------------------------------------------------
void Game::Startup()
{
    g_rng = new RandomNumberGenerator();

    ConfigureECS();
    g_ecs->Startup();
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

    g_ecs->RunFrame(deltaSeconds);
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
    g_ecs->Shutdown();

    delete g_rng;
    g_rng = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Game::ConfigureEngine(Engine* engine)
{
    EventSystemConfig eventSysConfig;
    g_eventSystem = new EventSystem(eventSysConfig);
    engine->RegisterSubsystem(g_eventSystem);

    WindowConfig windowConfig;
    windowConfig.m_windowTitle = "Louganis";
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
    jobSysConfig.m_threadCount = 12;
    g_jobSystem = new JobSystem(jobSysConfig);
    engine->RegisterSubsystem(g_jobSystem);

    JobSystemDebugConfig jobSysDebugConfig;
    jobSysDebugConfig.m_systemSubgraphNames = g_framePhaseNames;
    g_jobSystemDebug = new JobSystemDebug(jobSysDebugConfig);
    engine->RegisterSubsystem(g_jobSystemDebug);
}



//----------------------------------------------------------------------------------------------------------------------
void Game::ConfigureECS()
{
    AdminSystemConfig ecsConfig;
    ecsConfig.m_maxDeltaSeconds = 0.1f;
    ecsConfig.m_enableMultithreading = false;
    ecsConfig.m_autoMultithreadingEntityThreshold = 1;
    g_ecs = new AdminSystem(ecsConfig);

    // Array components
    g_ecs->RegisterComponentArray<CTransform>();
    g_ecs->RegisterComponentArray<CMovement>();
    g_ecs->RegisterComponentArray<CRender>();
    g_ecs->RegisterComponentArray<CCollision>();

    // Map components
    g_ecs->RegisterComponentMap<CCamera>();
    g_ecs->RegisterComponentMap<CPlayerController>();

    // Singleton components
    g_ecs->RegisterComponentSingleton<SCWorld>();
    g_ecs->RegisterComponentSingleton<SCEntityFactory>();
    g_ecs->RegisterComponentSingleton<SCFlowField>();
    g_ecs->RegisterComponentSingleton<SCDebug>();

    // Other resource types
    g_ecs->RegisterResourceByType<InputSystem>();
    g_ecs->RegisterResourceByType<Renderer>();

    // Pre Physics
    g_ecs->RegisterSystem<SEntityFactory>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SInput>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SWorld>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SLoadChunks>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SRemoveChunks>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SFlowField>((int) FramePhase::PrePhysics);

    // Physics
    SystemSubgraph& physics = g_ecs->CreateOrGetSystemSubgraph((int) FramePhase::Physics);
    physics.m_timeStep = 0.00833f;
    g_ecs->RegisterSystem<SMovement>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SCollision>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SWorldCollision>((int) FramePhase::Physics);

    // Render
    g_ecs->RegisterSystem<SCamera>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SCopyTransform>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderWorld>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderEntities>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SDebugRender>((int) FramePhase::Render);
}
