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
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
enum class FramePhase : int
{
    PrePhysics,
    Physics,
    PostPhysics,
};



//----------------------------------------------------------------------------------------------------------------------
void Game::Startup()
{
    AdminSystemConfig ecsConfig;
    ecsConfig.m_maxDeltaSeconds = 0.1f;
    g_ecs = new AdminSystem(ecsConfig);

    g_ecs->RegisterResourceByType<Renderer>(); // Calling draw or setting renderer state needs this
    
    g_ecs->RegisterComponent<CTransform>();
    g_ecs->RegisterComponent<CRender>();
    g_ecs->RegisterComponent<CCamera>(ComponentStorageType::MAP);
    
    g_ecs->RegisterSystem<SRender>((int) FramePhase::PostPhysics);

    SystemSubgraph& physics = g_ecs->GetSystemSubgraph((int) FramePhase::Physics);
    physics.m_timeStep = 0.00833f; // 120 Physics updates per second

    g_ecs->Startup();


    // Set up player
    auto player = g_ecs->CreateEntity();
    CRender& render = *g_ecs->AddComponent<CRender>(player);
    render.m_renderTransform.m_pos = Vec2(50.f, 25.f);
    std::vector<Vertex_PCU> verts;
    AddVertsForWireBox2D(verts, Vec2(-10.f, -10.f), Vec2(10.f, 10.f), 1.f);
    render.m_vertexBuffer.AddVerts(verts);
    CCamera& camera = *g_ecs->AddComponent<CCamera>(player);
    camera.m_camera.SetOrthoBounds(Vec3(0.f,0.f,0.f), Vec3(100.f, 50.f, 1.f));
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