// Bradley Christensen - 2024
#include "WorldRaycast.h"
#include "SCWorld.h"
#include "Chunk.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
WorldRaycastResult Raycast(SCWorld const& world, WorldRaycast const& raycast)
{
    WorldRaycastResult result;
    result.m_raycast = raycast;
    result.m_hitLocation = raycast.m_start;

    if (raycast.m_direction == Vec2::ZeroVector)
    {
        return result;
    }

    Chunk* startChunk = world.GetActiveChunkAtLocation(raycast.m_start);
    if (!startChunk)
    {
        return result;
    }

    result.m_hitLocation = raycast.m_start + raycast.m_direction * raycast.m_maxDistance;
    
    IntVec2 localTileCoords = world.GetLocalTileCoordsAtLocation(raycast.m_start);
    if (startChunk->IsTileSolid(localTileCoords))
    {
        result.m_blockingHit = true;
        result.m_immediateHit = true;
        result.m_hitNormal = -1.f * raycast.m_direction;
        return result;
    }

    AABB2 startTileBounds = world.GetTileBoundsAtWorldPos(raycast.m_start);
    Vec2 tileMinsToRayStart = raycast.m_start - startTileBounds.mins;

    float stepX = SignF(raycast.m_direction.x);
    float stepY = SignF(raycast.m_direction.y);
    
    float toLeadingEdgeX = stepX < 0 ? tileMinsToRayStart.x : world.m_worldSettings.m_tileWidth - tileMinsToRayStart.x;
    float toLeadingEdgeY = stepY < 0 ? tileMinsToRayStart.y : world.m_worldSettings.m_tileWidth - tileMinsToRayStart.y;

    float rayLengthPerStepX = AbsF(1 / raycast.m_direction.x);
    float rayLengthPerStepY = AbsF(1 / raycast.m_direction.y);

    IntVec2 currentTileCoords = localTileCoords;
    float totalRayLength = 0.f;

    // step along
    float totalRayLengthAfterStepX = totalRayLength + rayLengthPerStepX * toLeadingEdgeX;
    float totalRayLengthAfterStepY = totalRayLength + rayLengthPerStepY * toLeadingEdgeY;

    if (totalRayLengthAfterStepX < totalRayLengthAfterStepY)
    {
        currentTileCoords.x += stepX;
        totalRayLength = totalRayLengthAfterStepX;

        if (startChunk->IsTileSolid(currentTileCoords))
        {
            result.m_blockingHit = true;
            result.m_hitNormal = Vec2(-stepX, 0.f);
            result.m_hitLocation = result.m_raycast.m_start + result.m_raycast.m_direction * totalRayLength;
            return result;
        }
    }
    else
    {
        currentTileCoords.y += stepY;
        totalRayLength = totalRayLengthAfterStepY;

        if (startChunk->IsTileSolid(currentTileCoords))
        {
            result.m_blockingHit = true;
            result.m_hitNormal = Vec2(0.f, -stepY);
            result.m_hitLocation = result.m_raycast.m_start + result.m_raycast.m_direction * totalRayLength;
            return result;
        }
    }

    return result;
}



//----------------------------------------------------------------------------------------------------------------------
void DebugDrawRaycast(WorldRaycastResult const& result)
{
    VertexBuffer vbo;
    vbo.Initialize(1024);

    AddVertsForDisc2D(vbo.GetMutableVerts(), result.m_raycast.m_start, 0.1f, 16, Rgba8::Yellow);
    AddVertsForDisc2D(vbo.GetMutableVerts(), result.m_hitLocation, 0.1f, 16, Rgba8::Yellow);

    if (result.m_immediateHit)
    {
        AddVertsForArrow2D(vbo.GetMutableVerts(), result.m_raycast.m_start, result.m_hitLocation, 0.05f, Rgba8::Gray);
        AddVertsForArrow2D(vbo.GetMutableVerts(), result.m_raycast.m_start, result.m_raycast.m_start + result.m_hitNormal, 0.05f, Rgba8::Red);
    }
    else if (result.m_blockingHit)
    {
        AddVertsForArrow2D(vbo.GetMutableVerts(), result.m_raycast.m_start, result.m_hitLocation, 0.05f, Rgba8::Red);
        AddVertsForArrow2D(vbo.GetMutableVerts(), result.m_hitLocation, result.m_hitLocation + result.m_hitNormal, 0.05f, Rgba8::Red);
    }
    else
    {
        AddVertsForArrow2D(vbo.GetMutableVerts(), result.m_raycast.m_start, result.m_hitLocation, 0.05f, Rgba8::Yellow);
    }

    g_renderer->BindTexture(nullptr);
    g_renderer->BindShader(nullptr);
    g_renderer->DrawVertexBuffer(&vbo);
}
