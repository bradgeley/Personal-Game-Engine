// Bradley Christensen - 2023
#include "SRenderWorld.h"
#include "SCWorld.h"
#include "CCamera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Startup()
{
    AddWriteDependencies<SCWorld, Renderer>();
    AddReadDependencies<CCamera>();

    SCWorld& world = g_ecs->GetSingleton<SCWorld>();

    auto& tiles = world.m_tileIds;
    world.m_tileVbo = new VertexBuffer();

    Vec2 worldDims = Vec2(tiles.GetDimensions()) * world.m_tileSize;

    AABB2 worldBounds;
    worldBounds.mins = world.m_offset;
    worldBounds.maxs = worldBounds.mins + worldDims;
    AddVertsForGrid2D(world.m_tileVbo->GetMutableVerts(), worldBounds, tiles.GetDimensions(), Rgba8::Red);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Run(SystemContext const& context)
{
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();

    auto& vbo = world.m_tileVbo;

    for (auto it = g_ecs->Iterate<CCamera>(context); it.IsValid(); ++it)
    {
        CCamera* cameraComponent = g_ecs->GetComponent<CCamera>(it.m_currentIndex);
        g_renderer->BeginCamera(&cameraComponent->m_camera);
        g_renderer->ClearScreen(Rgba8::White);
        g_renderer->BindTexture(nullptr);
        g_renderer->BindShader(nullptr);
        g_renderer->DrawVertexBuffer(vbo);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Shutdown()
{
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();

    delete world.m_tileVbo;
    world.m_tileVbo = nullptr;
}