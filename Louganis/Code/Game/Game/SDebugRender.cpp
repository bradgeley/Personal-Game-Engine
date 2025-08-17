// Bradley Christensen - 2023
#include "SDebugRender.h"
#include "CTransform.h"
#include "CPlayerController.h"
#include "SCWorld.h"
#include "SCFlowField.h"
#include "FlowField.h"
#include "FlowFieldChunk.h"
#include "Chunk.h"
#include "WorldRaycast.h"
#include "CCamera.h"
#include "SCDebug.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Events/EventSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Startup()
{
    AddWriteDependencies<CTransform, CPlayerController, Renderer>();
    AddWriteDependencies<SCDebug>();
    AddReadDependencies<SCWorld, SCFlowField, InputSystem, CCamera>();

    g_eventSystem->SubscribeMethod("DebugRenderMouseRaycast", this, &SDebugRender::DebugRenderMouseRaycast);
    g_eventSystem->SubscribeMethod("DebugRenderCostField", this, &SDebugRender::DebugRenderCostField);
    g_eventSystem->SubscribeMethod("DebugRenderDistanceField", this, &SDebugRender::DebugRenderDistanceField);
    g_eventSystem->SubscribeMethod("DebugRenderFlowField", this, &SDebugRender::DebugRenderFlowField);
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& cameraStorage = g_ecs->GetMapStorage<CCamera>();
    SCWorld const& world = g_ecs->GetSingleton<SCWorld>();
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    SCFlowField const& scFlowfield = g_ecs->GetSingleton<SCFlowField>();
    FlowField const& flowField = scFlowfield.m_toPlayerFlowField;
    Font* font = g_renderer->GetDefaultFont();

    auto playerCameraIt = g_ecs->Iterate<CTransform, CPlayerController, CCamera>(context);
    if (playerCameraIt.IsValid())
    {
        CCamera* cameraComp = cameraStorage.Get(playerCameraIt);
        g_renderer->BeginCamera(&cameraComp->m_camera);
    }

    if (scDebug.m_debugRenderToMouseRaycast)
    {
        for (auto it = g_ecs->Iterate<CTransform, CPlayerController>(context); it.IsValid(); ++it)
        {
            CTransform* playerTransform = transStorage.Get(it);
            CCamera* cameraComp = cameraStorage.Get(it);
            Vec2 playerLocation = playerTransform->m_pos;
            Vec2 relMousePos = g_input->GetMouseClientRelativePosition();
            Vec2 worldMousePos = cameraComp->m_camera.ScreenToWorldOrtho(relMousePos);
            Vec2 playerToMouse = worldMousePos - playerLocation;
            WorldRaycast raycast(playerLocation, playerToMouse.GetNormalized(), playerToMouse.GetLength());
            WorldRaycastResult result = Raycast(world, raycast);

            AddVertsForRaycast(scDebug.m_frameVerts, result);
        }
    }

    // Render Cost Field
    if (scDebug.m_debugRenderCostField)
    {
        for (auto const& it : flowField.m_activeFlowFieldChunks)
        {
            FlowFieldChunk* ffChunk = it.second;
            WorldCoords currentWorldCoords;
            currentWorldCoords.m_chunkCoords = ffChunk->GetChunkCoords();

            ffChunk->m_debugVBO.ClearVerts();
            for (int y = 0; y < ffChunk->m_costField.GetHeight(); ++y)
            {
                for (int x = 0; x < ffChunk->m_costField.GetWidth(); ++x)
                {
                    currentWorldCoords.m_localTileCoords = IntVec2(x, y);
                    AABB2 tileBounds = world.GetTileBounds(currentWorldCoords);
                    uint8_t cost = ffChunk->m_costField.Get(x, y);
                    float t = MathUtils::RangeMapClamped((float) cost, 0.f, 255.f, 0.f, 1.f);
                    Rgba8 tint = Rgba8::Lerp(Rgba8(255, 255, 255, 150), Rgba8(0, 0, 0, 150), t);
                    AddVertsForAABB2(ffChunk->m_debugVBO.GetMutableVerts(), tileBounds, tint);
                }
            }

            g_renderer->BindShader(nullptr);
            g_renderer->BindTexture(nullptr);
            g_renderer->DrawVertexBuffer(&ffChunk->m_debugVBO);
        }
    }


    // Render Flow Field
    if (scDebug.m_debugRenderDistanceField)
    {
        for (auto it : flowField.m_activeFlowFieldChunks)
        {
            FlowFieldChunk* ffChunk = it.second;
            WorldCoords currentWorldCoords;
            currentWorldCoords.m_chunkCoords = ffChunk->GetChunkCoords();

            ffChunk->m_debugVBO.ClearVerts();
            for (int y = 0; y < ffChunk->m_distanceField.GetHeight(); ++y)
            {
                for (int x = 0; x < ffChunk->m_distanceField.GetWidth(); ++x)
                {
                    currentWorldCoords.m_localTileCoords = IntVec2(x, y);
                    AABB2 tileBounds = world.GetTileBounds(currentWorldCoords);
                    float distance = ffChunk->m_distanceField.Get(x, y);

                    float t = MathUtils::RangeMapClamped(distance, 0.f, 10.f, 0.f, 1.f);
                    Rgba8 const& tint = Rgba8::Lerp(Rgba8::Green, Rgba8::Red, t);
                    std::string distanceText = StringUtils::StringF("%.2f", distance);
                    font->AddVertsForAlignedText2D(ffChunk->m_debugVBO.GetMutableVerts(), tileBounds.GetCenter(), Vec2::ZeroVector, 0.5f, distanceText, tint);
                }
            }

            font->SetRendererState();
            g_renderer->DrawVertexBuffer(&ffChunk->m_debugVBO);
        }
    }


    // Render Gradient
    if (scDebug.m_debugRenderGradient)
    {
        for (auto it : flowField.m_activeFlowFieldChunks)
        {
            FlowFieldChunk* ffChunk = it.second;
            WorldCoords currentWorldCoords;
            currentWorldCoords.m_chunkCoords = ffChunk->GetChunkCoords();

            ffChunk->m_debugVBO.ClearVerts();
            for (int y = 0; y < ffChunk->m_distanceField.GetHeight(); ++y)
            {
                for (int x = 0; x < ffChunk->m_distanceField.GetWidth(); ++x)
                {
                    currentWorldCoords.m_localTileCoords = IntVec2(x, y);
                    AABB2 tileBounds = world.GetTileBounds(currentWorldCoords);
                    Vec2 gradient = ffChunk->m_gradient.Get(x, y);

                    AddVertsForArrow2D(ffChunk->m_debugVBO.GetMutableVerts(), tileBounds.GetCenter(), tileBounds.GetCenter() + gradient, 0.05f, Rgba8::Yellow);
                }
            }

            g_renderer->BindTexture(0);
            g_renderer->BindShader(0);
            g_renderer->DrawVertexBuffer(&ffChunk->m_debugVBO);
        }
    }
    
    // Render debug verts 
    g_renderer->BindTexture(0);
    g_renderer->BindShader(0);
    g_renderer->DrawVertexBuffer(&scDebug.m_frameVerts);
    scDebug.m_frameVerts.ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Shutdown()
{
    g_eventSystem->UnsubscribeMethod("DebugRenderMouseRaycast", this, &SDebugRender::DebugRenderMouseRaycast);
    g_eventSystem->UnsubscribeMethod("DebugRenderCostField", this, &SDebugRender::DebugRenderCostField);
    g_eventSystem->UnsubscribeMethod("DebugRenderDistanceField", this, &SDebugRender::DebugRenderDistanceField);
    g_eventSystem->UnsubscribeMethod("DebugRenderFlowField", this, &SDebugRender::DebugRenderFlowField);
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugRenderMouseRaycast(NamedProperties&)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderToMouseRaycast = !scDebug.m_debugRenderToMouseRaycast;
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugRenderCostField(NamedProperties&)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderCostField = !scDebug.m_debugRenderCostField;
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugRenderDistanceField(NamedProperties&)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderDistanceField = !scDebug.m_debugRenderDistanceField;
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugRenderFlowField(NamedProperties&)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderGradient = !scDebug.m_debugRenderGradient;
    return false;
}
