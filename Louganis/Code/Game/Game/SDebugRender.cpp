// Bradley Christensen - 2023
#include "SDebugRender.h"
#include "CTransform.h"
#include "CPlayerController.h"
#include "SCWorld.h"
#include "SCFlowField.h"
#include "FlowFieldChunk.h"
#include "Chunk.h"
#include "WorldRaycast.h"
#include "CCamera.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Startup()
{
    AddWriteDependencies<CTransform, CPlayerController>();
    AddReadDependencies<SCWorld, SCFlowField, InputSystem, CCamera>();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& cameraStorage = g_ecs->GetMapStorage<CCamera>();
    SCWorld const& world = g_ecs->GetSingleton<SCWorld>();

    for (auto it = g_ecs->Iterate<CTransform, CPlayerController, CCamera>(context); it.IsValid(); ++it)
    {
        CTransform* playerTransform = transStorage.Get(it);
        CCamera* cameraComp = cameraStorage.Get(it);
        Vec2 playerLocation = playerTransform->m_pos;
        Vec2 relMousePos = g_input->GetMouseClientRelativePosition();
        Vec2 worldMousePos = cameraComp->m_camera.ScreenToWorldOrtho(relMousePos);
        Vec2 playerToMouse = worldMousePos - playerLocation;
        WorldRaycast raycast(playerLocation, playerToMouse.GetNormalized(), playerToMouse.GetLength());
        WorldRaycastResult result = Raycast(world, raycast);

        g_renderer->BeginCamera(&cameraComp->m_camera);
        DebugDrawRaycast(result);
    }

    // Render Cost Field
    SCFlowField const& flowfield = g_ecs->GetSingleton<SCFlowField>();
    for (auto it : flowfield.m_activeFlowFieldChunks)
    {
        FlowFieldChunk* ffChunk = it.second;

        ffChunk->m_debugVBO.ClearVerts();
        for (int y = 0; y < ffChunk->m_costField.GetHeight(); ++y)
        {
            for (int x = 0; x < ffChunk->m_costField.GetWidth(); ++x)
            {
                IntVec2 localTileCoords(x, y);
                AABB2 tileBounds = world.GetTileBounds(ffChunk->m_chunk->m_chunkCoords, localTileCoords);
                uint8_t cost = ffChunk->m_costField.Get(x, y);
                float t = RangeMapClamped((float) cost, 0.f, 255.f, 0.f, 1.f);
                Rgba8 tint = Rgba8::Lerp(Rgba8(255, 255, 255, 150), Rgba8(0, 0, 0, 150), t);
                AddVertsForAABB2(ffChunk->m_debugVBO.GetMutableVerts(), tileBounds, tint);
            }
        }

        g_renderer->BindShader(nullptr);
        g_renderer->BindTexture(nullptr);
        g_renderer->DrawVertexBuffer(&ffChunk->m_debugVBO);
    }

    // Render Flow Field
    Font* font = g_renderer->GetDefaultFont();
    for (auto it : flowfield.m_activeFlowFieldChunks)
    {
        FlowFieldChunk* ffChunk = it.second;
    
        ffChunk->m_debugVBO.ClearVerts();
        for (int y = 0; y < ffChunk->m_distanceField.GetHeight(); ++y)
        {
            for (int x = 0; x < ffChunk->m_distanceField.GetWidth(); ++x)
            {
                IntVec2 localTileCoords(x, y);
                AABB2 tileBounds = world.GetTileBounds(ffChunk->m_chunk->m_chunkCoords, localTileCoords);
                float distance = ffChunk->m_distanceField.Get(x, y);
    
                float t = RangeMapClamped(distance, 0.f, 10.f, 0.f, 1.f);
                Rgba8 const& tint = Rgba8::Lerp(Rgba8::Green, Rgba8::Red, t);
                std::string distanceText = StringF("%.2f", distance);
                font->AddVertsForAlignedText2D(ffChunk->m_debugVBO.GetMutableVerts(), tileBounds.GetCenter(), Vec2::ZeroVector, 0.5f, distanceText, tint);
            }
        }
    
        font->SetRendererState();
        g_renderer->DrawVertexBuffer(&ffChunk->m_debugVBO);
    }

    // Render Gradient

    for (auto it : flowfield.m_activeFlowFieldChunks)
    {
        FlowFieldChunk* ffChunk = it.second;
    
        ffChunk->m_debugVBO.ClearVerts();
        for (int y = 0; y < ffChunk->m_distanceField.GetHeight(); ++y)
        {
            for (int x = 0; x < ffChunk->m_distanceField.GetWidth(); ++x)
            {
                IntVec2 localTileCoords(x, y);
                AABB2 tileBounds = world.GetTileBounds(ffChunk->m_chunk->m_chunkCoords, localTileCoords);
                Vec2 gradient = ffChunk->m_gradient.Get(x, y);
    
                AddVertsForArrow2D(ffChunk->m_debugVBO.GetMutableVerts(), tileBounds.GetCenter(), tileBounds.GetCenter() + gradient, 0.05f, Rgba8::Yellow);
            }
        }
    
        g_renderer->BindTexture(0);
        g_renderer->BindShader(0);
        g_renderer->DrawVertexBuffer(&ffChunk->m_debugVBO);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Shutdown()
{
    
}