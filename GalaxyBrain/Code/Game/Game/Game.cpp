// Bradley Christensen - 2022-2023
#include "Game.h"
#include "GameCommon.h"
#include "WindowsApplication.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexUtils.h"
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

    g_ecs->RegisterResourceByType<Renderer>(); // Calling draw or setting renderer state needs this
    
    g_ecs->RegisterComponent<CTransform>();
    g_ecs->RegisterComponent<CMovement>();
    g_ecs->RegisterComponent<CPhysics>();
    g_ecs->RegisterComponent<CRender>();
    g_ecs->RegisterComponent<CCamera>(ComponentStorageType::MAP);
    g_ecs->RegisterComponent<SCEntityFactory>(ComponentStorageType::SINGLETON);
    g_ecs->RegisterResourceByType<InputSystem>();

    // Pre Physics
    g_ecs->RegisterSystem<SEntityFactory>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SInput>((int) FramePhase::PrePhysics);
    
    // Physics
    SystemSubgraph& physics = g_ecs->GetSystemSubgraph((int) FramePhase::Physics);
    physics.m_timeStep = 0.00833f;
    g_ecs->RegisterSystem<SMovement>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SPhysics>((int) FramePhase::Physics);
    
    // Post Physics
    
    // Render
    g_ecs->RegisterSystem<SRender>((int) FramePhase::Render);
    
    g_ecs->Startup();
    
    // Set up player
    EntityDefinition playerDef = g_ecs->NewEntityDef();
    CPhysics& phys = *playerDef.GetComponentDef<CPhysics>();
    phys.m_gravityStrength = 1.f;
    CMovement& move = *playerDef.GetComponentDef<CMovement>();
    move.m_movementSpeed = 10.f;
    CRender& render = *playerDef.GetComponentDef<CRender>();
    AddVertsForRect2D(render.m_vertexBuffer.GetMutableVerts(), Vec2(-5.f, -10.f), Vec2(5.f, 10.f));
    CCamera& camera = *playerDef.GetComponentDef<CCamera>();
    camera.m_camera.SetOrthoBounds(Vec3(0.f,0.f,0.f), Vec3(100.f, 50.f, 1.f));

    auto factory = g_ecs->GetComponent<SCEntityFactory>();
    factory->m_entityDefinitions["Player"] = playerDef;
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
    if (g_input->WasKeyJustReleased(KeyCode::Escape))
    {
        g_app->Quit();
    }

    if (g_input->WasKeyJustReleased(' '))
    {
        g_ecs->DestroyEntity(0);
        auto factory = g_ecs->GetComponent<SCEntityFactory>();
        SpawnInfo playerInfo;
        playerInfo.m_spawnPos = Vec2(50.f, 25.f);
        playerInfo.m_def = &factory->m_entityDefinitions["Player"];
        factory->m_entitiesToSpawn.emplace_back(playerInfo);
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