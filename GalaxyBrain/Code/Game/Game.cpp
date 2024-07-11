// Bradley Christensen - 2022-2023
#include "Game.h"
#include "GameCommon.h"
#include "WindowsApplication.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Multithreading/JobSystem.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Core/Engine.h"
#include "AllComponents.h"
#include "AllSystems.h"



//----------------------------------------------------------------------------------------------------------------------
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

    AdminSystemConfig ecsConfig;
    ecsConfig.m_maxDeltaSeconds = 0.1f;
    g_ecs = new AdminSystem(ecsConfig);
    
    // Array components
    g_ecs->RegisterComponentArray<CTransform>();
    g_ecs->RegisterComponentArray<CMovement>();
    g_ecs->RegisterComponentArray<CPhysics>();
    g_ecs->RegisterComponentArray<CRender>();
    g_ecs->RegisterComponentArray<CCollision>();

    // Map components
    g_ecs->RegisterComponentMap<CCamera>();
    g_ecs->RegisterComponentMap<CPlayerController>();

    // Singleton components
    g_ecs->RegisterComponentSingleton<SCEntityFactory>();
    g_ecs->RegisterComponentSingleton<SCRenderer>();
    g_ecs->RegisterComponentSingleton<SCUniverse>();
    g_ecs->RegisterComponentSingleton<SCDebug>();

    // Other resource types
    g_ecs->RegisterResourceByType<InputSystem>();
    g_ecs->RegisterResourceByType<Renderer>();

    // Pre Physics
    g_ecs->RegisterSystem<SDebugKeys>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SEntityFactory>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SInput>((int) FramePhase::PrePhysics);
    
    // Physics
    SystemSubgraph& physics = g_ecs->GetSystemSubgraph((int) FramePhase::Physics);
    physics.m_timeStep = 0.00833f;
    g_ecs->RegisterSystem<SGravity>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SMovement>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SPhysics>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SCollision>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SCamera>((int) FramePhase::Physics); // Camera is here because it does framerate dependent things
    
    // Post Physics
    
    // Render
    g_ecs->RegisterSystem<SBackgroundStar>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SRender>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderDebug>((int) FramePhase::Render);
    
    g_ecs->Startup();

    RegisterDevConsoleCommands();
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
    if (g_input->WasKeyJustReleased(KeyCode::Escape))
    {
        g_app->Quit();
    }

    if (g_input->IsKeyDown('T'))
    {
        deltaSeconds *= 10.f;
    }

    // g_ecs->RunFrame(deltaSeconds);
    g_ecs->RunSystemSubgraph((int) FramePhase::PrePhysics, deltaSeconds);
    g_ecs->RunSystemSubgraph((int) FramePhase::Physics, deltaSeconds);
    g_ecs->RunSystemSubgraph((int) FramePhase::PostPhysics, deltaSeconds);
    g_ecs->RunSystemSubgraph((int) FramePhase::Render, deltaSeconds);
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
    UnRegisterDevConsoleCommands();
    g_ecs->Shutdown();

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
    windowConfig.m_windowTitle = "Galaxy Brain";
    windowConfig.m_clientAspect = 2.f;
    windowConfig.m_windowScale = 0.95f;
    g_window = new Window(windowConfig);
    engine->RegisterSubsystem(g_window);

    RendererConfig rendererConfig;
    g_renderer = new Renderer(rendererConfig);
    engine->RegisterSubsystem(g_renderer);

    // Dev console before input, so it steals input from the window when active
    DevConsoleConfig dcConfig;
    dcConfig.m_backgroundImages = { "DrStrange.jpg" , "Thanos.jpg", "IronMan.jpg", "Thor.jpg" };
    g_devConsole = new DevConsole(dcConfig);
    engine->RegisterSubsystem(g_devConsole);

    InputSystemConfig inputConfig;
    g_input = new InputSystem(inputConfig);
    engine->RegisterSubsystem(g_input);

    JobSystemConfig jobSysConfig;
    g_jobSystem = new JobSystem(jobSysConfig);
    engine->RegisterSubsystem(g_jobSystem);
}



//----------------------------------------------------------------------------------------------------------------------
void Game::RegisterDevConsoleCommands() const
{
    SubscribeEventCallbackFunction("SystemActive", Game::OnSystemActiveCommand);
    DevConsoleCommandInfo systemActiveInfo("SystemActive");
    systemActiveInfo.AddArg("on", DevConsoleArgType::Bool);
    g_devConsole->AddDevConsoleCommandInfo(systemActiveInfo);
}



//----------------------------------------------------------------------------------------------------------------------
void Game::UnRegisterDevConsoleCommands() const
{
}



//----------------------------------------------------------------------------------------------------------------------
bool Game::OnSystemActiveCommand(NamedProperties& eventArgs)
{
    std::string name = eventArgs.Get<std::string>("name", "");
    bool active = eventArgs.Get("on", true);
    g_ecs->SetSystemActive(name, active);
    return true;
}
