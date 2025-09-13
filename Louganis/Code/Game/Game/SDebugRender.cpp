// Bradley Christensen - 2022-2025
#include "SDebugRender.h"
#include "CTransform.h"
#include "CPlayerController.h"
#include "SCWorld.h"
#include "SCFlowField.h"
#include "SCCollision.h"
#include "FlowField.h"
#include "FlowFieldChunk.h"
#include "Chunk.h"
#include "WorldRaycast.h"
#include "CCamera.h"
#include "SCDebug.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Startup()
{
    AddWriteDependencies<CTransform, CPlayerController, Renderer>();
    AddWriteDependencies<SCDebug>();
    AddReadDependencies<SCWorld, SCFlowField, SCCollision, InputSystem, CCamera>();

    g_eventSystem->SubscribeMethod("DebugRenderMouseRaycast", this, &SDebugRender::DebugRenderMouseRaycast);
    g_eventSystem->SubscribeMethod("DebugRenderMouseDiscCast", this, &SDebugRender::DebugRenderMouseDiscCast);
    g_eventSystem->SubscribeMethod("DebugRenderCostField", this, &SDebugRender::DebugRenderCostField);
    g_eventSystem->SubscribeMethod("DebugRenderDistanceField", this, &SDebugRender::DebugRenderDistanceField);
    g_eventSystem->SubscribeMethod("DebugRenderFlowField", this, &SDebugRender::DebugRenderFlowField);
    g_eventSystem->SubscribeMethod("DebugRenderSolidTiles", this, &SDebugRender::DebugRenderSolidTiles);
    g_eventSystem->SubscribeMethod("DebugRenderCollision", this, &SDebugRender::DebugRenderCollision);

    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_frameUntexVerts = g_renderer->MakeVertexBuffer();
    scDebug.m_frameDefaultFontVerts = g_renderer->MakeVertexBuffer();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& cameraStorage = g_ecs->GetMapStorage<CCamera>();
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    SCFlowField const& scFlowfield = g_ecs->GetSingleton<SCFlowField>();
    FlowField const& flowField = scFlowfield.m_toPlayerFlowField;
    Font* font = g_renderer->GetDefaultFont();

    VertexBuffer& frameVerts = *g_renderer->GetVertexBuffer(scDebug.m_frameUntexVerts);
    VertexBuffer& frameTextVerts = *g_renderer->GetVertexBuffer(scDebug.m_frameDefaultFontVerts);
    CCamera* playerCamera = nullptr;

    if (g_window->HasFocus())
    {
        auto it = g_ecs->Iterate<CTransform, CPlayerController>(context);
        if (it.IsValid())
        {
            playerCamera = cameraStorage.Get(it);
            Vec2 relMousePos = g_input->GetMouseClientRelativePosition();
            scDebug.m_debugMouseLocation = playerCamera->m_camera.ScreenToWorldOrtho(relMousePos);
        }
    }

    // Mouse Raycast
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

            AddVertsForRaycast(frameVerts, result);
        }
    }

    // Mouse Disc Cast
    if (scDebug.m_debugRenderToMouseDiscCast)
    {
        for (auto it = g_ecs->Iterate<CTransform, CPlayerController>(context); it.IsValid(); ++it)
        {
            CTransform* playerTransform = transStorage.Get(it);
            Vec2 playerLocation = playerTransform->m_pos;
            Vec2 playerToMouse = scDebug.m_debugMouseLocation - playerLocation;
            WorldDiscCast discCast; 
            discCast.m_direction = playerToMouse.GetNormalized();
            discCast.m_start = playerLocation;
            discCast.m_maxDistance = playerToMouse.GetLength();
            discCast.m_discRadius = 10.f;

            WorldDiscCastResult result;
            // Disc Cast
            {
                ScopedTimer t("DiscCast - RenderDebug");
                result = DiscCast(world, discCast);
            }

            AABB2 capsuleBounds = GeometryUtils::GetCapsuleBounds(result.m_discCast.m_start, result.m_newDiscCenter, result.m_discCast.m_discRadius);
            VertexUtils::AddVertsForWireBox2D(frameVerts, capsuleBounds, 0.1f);
            std::vector<Chunk*> chunks;
            world.GetChunksOverlappingAABB(chunks, capsuleBounds);
            for (auto& chunk : chunks)
            {
                if (GeometryUtils::DoesCapsuleOverlapAABB(result.m_discCast.m_start, result.m_newDiscCenter, result.m_discCast.m_discRadius, chunk->m_chunkBounds))
                {
                    Rgba8 tint = Rgba8::Cyan;
                    VertexUtils::AddVertsForWireBox2D(frameVerts, chunk->m_chunkBounds, 0.1f, tint);
                }
            }

            world.ForEachWorldCoordsOverlappingCapsule(result.m_discCast.m_start, result.m_newDiscCenter, result.m_discCast.m_discRadius, [&](WorldCoords const& coords)
            {
                VertexUtils::AddVertsForWireBox2D(frameVerts, world.GetTileBounds(coords), 0.1f, Rgba8::Magenta);
                return true; 
            });

            AddVertsForDiscCast(frameVerts, result);
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

            for (int y = 0; y < ffChunk->m_costField.GetHeight(); ++y)
            {
                for (int x = 0; x < ffChunk->m_costField.GetWidth(); ++x)
                {
                    currentWorldCoords.m_localTileCoords = IntVec2(x, y);
                    AABB2 tileBounds = world.GetTileBounds(currentWorldCoords);
                    uint8_t cost = ffChunk->m_costField.Get(x, y);
                    float t = MathUtils::RangeMapClamped((float) cost, 0.f, 255.f, 0.f, 1.f);
                    Rgba8 tint = Rgba8::Lerp(Rgba8(255, 255, 255, 150), Rgba8(0, 0, 0, 150), t);
                    VertexUtils::AddVertsForAABB2(frameVerts, tileBounds, tint);
                }
            }
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
                    font->AddVertsForAlignedText2D(frameTextVerts, tileBounds.GetCenter(), Vec2::ZeroVector, 0.5f, distanceText, tint);
                }
            }
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

            for (int y = 0; y < ffChunk->m_distanceField.GetHeight(); ++y)
            {
                for (int x = 0; x < ffChunk->m_distanceField.GetWidth(); ++x)
                {
                    currentWorldCoords.m_localTileCoords = IntVec2(x, y);
                    AABB2 tileBounds = world.GetTileBounds(currentWorldCoords);
                    Vec2 gradient = ffChunk->m_gradient.Get(x, y);

                    VertexUtils::AddVertsForArrow2D(frameVerts, tileBounds.GetCenter(), tileBounds.GetCenter() + gradient, 0.05f, Rgba8::Yellow);
                }
            }
        }
    }

    // Render Solid Tiles
    if (scDebug.m_debugRenderSolidTiles && playerCamera)
    {
        world.ForEachChunkOverlappingAABB(playerCamera->m_camera.GetTranslatedOrthoBounds2D(), [&world, &frameVerts](Chunk& chunk)
        {
            for (int tileID = 0; tileID < s_numTilesInChunk; ++tileID)
            {
                if (chunk.IsTileSolid(tileID))
                {
                    WorldCoords worldCoords;
                    worldCoords.m_chunkCoords = chunk.m_chunkCoords;
                    worldCoords.m_localTileCoords = chunk.m_tileIDs.GetCoordsForIndex(tileID);
                    VertexUtils::AddVertsForAABB2(frameVerts, world.GetTileBounds(worldCoords));
                }
            }
            return true;
        });
    }

    // Render Collision Stuff
    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();
    if (scDebug.m_debugRenderCollision)
    {
		VertexUtils::AddVertsForWireBox2D(frameVerts, scCollision.m_collisionUpdateBounds, 0.25f, Rgba8::Magenta);

        for (auto const& chunkCollisionDataPair : scCollision.m_chunkCollisionData)
        {
            IntVec2 const& chunkCoords = chunkCollisionDataPair.first;
			Chunk* chunk = world.GetActiveChunk(chunkCoords);
            if (!chunk)
            {
                continue;
            }
            ChunkCollisionData const& chunkCollisionData = chunkCollisionDataPair.second;
			auto& chunkBucket = chunkCollisionData.m_chunkBucket;
			font->AddVertsForAlignedText2D(frameTextVerts, chunk->m_chunkBounds.GetCenter(), Vec2::ZeroVector, 0.5f, StringUtils::StringF("Chunk (%d, %d): Entities hashed: %d", chunkCoords.x, chunkCoords.y, chunkBucket.size()), Rgba8::White);
			VertexUtils::AddVertsForWireBox2D(frameVerts, chunk->m_chunkBounds, 0.1f, Rgba8::Cyan);
        
            for (auto const& tileBucketPair : chunkCollisionData.m_tileBuckets)
            {
                int tileIndex = tileBucketPair.first;
				EntityBucket const& tileBucket = tileBucketPair.second;
				WorldCoords worldCoords = WorldCoords(chunkCoords, chunk->m_tileIDs.GetCoordsForIndex(tileIndex));
                AABB2 tileBounds = world.GetTileBounds(worldCoords);
				font->AddVertsForAlignedText2D(frameTextVerts, tileBounds.GetCenter(), Vec2::ZeroVector, 0.5f, StringUtils::StringF("%d", tileBucket.size()), Rgba8::White);
                VertexUtils::AddVertsForWireBox2D(frameVerts, tileBounds, 0.05f, Rgba8::Yellow);
            }
		}
    }

    
    // Render->clear debug verts 
    g_renderer->BindTexture(nullptr);
    g_renderer->BindShader(nullptr);
    g_renderer->DrawVertexBuffer(frameVerts);
    frameVerts.ClearVerts();

    // Render->clear debug text verts 
    font->SetRendererState();
    g_renderer->DrawVertexBuffer(frameTextVerts);
    frameTextVerts.ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Shutdown()
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();

    g_renderer->ReleaseVertexBuffer(scDebug.m_frameDefaultFontVerts);
    g_renderer->ReleaseVertexBuffer(scDebug.m_frameUntexVerts);

    g_eventSystem->UnsubscribeMethod("DebugRenderMouseRaycast", this, &SDebugRender::DebugRenderMouseRaycast);
    g_eventSystem->UnsubscribeMethod("DebugRenderMouseDiscCast", this, &SDebugRender::DebugRenderMouseDiscCast);
    g_eventSystem->UnsubscribeMethod("DebugRenderCostField", this, &SDebugRender::DebugRenderCostField);
    g_eventSystem->UnsubscribeMethod("DebugRenderDistanceField", this, &SDebugRender::DebugRenderDistanceField);
    g_eventSystem->UnsubscribeMethod("DebugRenderFlowField", this, &SDebugRender::DebugRenderFlowField);
    g_eventSystem->UnsubscribeMethod("DebugRenderSolidTiles", this, &SDebugRender::DebugRenderSolidTiles);
    g_eventSystem->UnsubscribeMethod("DebugRenderCollision", this, &SDebugRender::DebugRenderCollision);
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugRenderMouseRaycast(NamedProperties&)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderToMouseRaycast = !scDebug.m_debugRenderToMouseRaycast;
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugRenderMouseDiscCast(NamedProperties&)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderToMouseDiscCast = !scDebug.m_debugRenderToMouseDiscCast;
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



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugRenderSolidTiles(NamedProperties&)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderSolidTiles = !scDebug.m_debugRenderSolidTiles;
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugRenderCollision(NamedProperties&)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderCollision = !scDebug.m_debugRenderCollision;
    return false;
}
