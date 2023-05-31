// Bradley Christensen - 2022-2023
#include "Game.h"
#include "GameCommon.h"
#include "WindowsApplication.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/Renderer/Renderer.h"
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
    AdminSystemConfig ecsConfig;
    ecsConfig.m_maxDeltaSeconds = 0.1f;
    g_ecs = new AdminSystem(ecsConfig);
    
    g_ecs->RegisterComponentArray<CTransform>();
    g_ecs->RegisterComponentArray<CMovement>();
    g_ecs->RegisterComponentArray<CPhysics>();
    g_ecs->RegisterComponentArray<CRender>();
    g_ecs->RegisterComponentMap<CCamera>();
    g_ecs->RegisterComponentSingleton<SCEntityFactory>();
    g_ecs->RegisterComponentSingleton<SCRenderer>();
    g_ecs->RegisterResourceByType<InputSystem>();
    g_ecs->RegisterResourceByType<Renderer>();

    // Pre Physics
    g_ecs->RegisterSystem<SDebugKeys>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SEntityFactory>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SInput>((int) FramePhase::PrePhysics);
    
    // Physics
    SystemSubgraph& physics = g_ecs->GetSystemSubgraph((int) FramePhase::Physics);
    physics.m_timeStep = 0.00833f;
    g_ecs->RegisterSystem<SMovement>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SPhysics>((int) FramePhase::Physics);
    
    // Post Physics
    g_ecs->RegisterSystem<SCamera>((int) FramePhase::PostPhysics);
    
    // Render
    g_ecs->RegisterSystem<SRender>((int) FramePhase::Render);
    
    g_ecs->Startup();
    
    // Set up player
    //EntityDefinition playerDef = g_ecs->NewEntityDef();
    //CPhysics& phys = *playerDef.GetComponentDef<CPhysics>();
    //phys.m_gravityStrength = 1.f;
    //CMovement& move = *playerDef.GetComponentDef<CMovement>();
    //move.m_movementSpeed = 10.f;
    //CRender& render = *playerDef.GetComponentDef<CRender>();
    //CCamera& camera = *playerDef.GetComponentDef<CCamera>();
    //camera.m_camera.SetOrthoBounds(Vec3(0.f,0.f,0.f), Vec3(100.f, 50.f, 1.f));
    //
    //auto factory = g_ecs->GetComponent<SCEntityFactory>();
    //factory->m_entityDefinitions["Player"] = playerDef;
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
    if (g_input->WasKeyJustReleased(KeyCode::Escape))
    {
        g_app->Quit();
    }


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
    g_ecs->Shutdown();
}