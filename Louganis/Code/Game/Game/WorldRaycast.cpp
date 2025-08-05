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
    
    WorldCoords currentWorldCoords = world.GetWorldCoordsAtLocation(raycast.m_start);
    if (startChunk->IsTileSolid(currentWorldCoords.m_localTileCoords))
    {
        // Somehow we got inside a block
        result.m_blockingHit = true;
        result.m_immediateHit = true;
        result.m_hitNormal = -raycast.m_direction;
        result.m_hitLocation = result.m_raycast.m_start;
        result.m_t = 0.f;
        result.m_distance = 0.f;
        return result;
    }

    AABB2 startTileBounds = world.GetTileBounds(currentWorldCoords);
    Vec2 tileMinsToRayStart = raycast.m_start - startTileBounds.mins;

    float stepX = SignF(raycast.m_direction.x);
    float stepY = SignF(raycast.m_direction.y);
    
    float toLeadingEdgeX = stepX < 0.f ? tileMinsToRayStart.x : world.m_worldSettings.m_tileWidth - tileMinsToRayStart.x;
    float toLeadingEdgeY = stepY < 0.f ? tileMinsToRayStart.y : world.m_worldSettings.m_tileWidth - tileMinsToRayStart.y;

    float rayLengthPerStepX = AbsF(1.f / raycast.m_direction.x);
    float rayLengthPerStepY = AbsF(1.f / raycast.m_direction.y);

    float totalRayLength = 0.f;
    float totalRayLengthX = rayLengthPerStepX * toLeadingEdgeX;
    float totalRayLengthY = rayLengthPerStepY * toLeadingEdgeY;

    Chunk* currentChunk = startChunk;

    while (totalRayLength < result.m_raycast.m_maxDistance)
    {
        if (totalRayLengthX < totalRayLengthY)
        {
            currentWorldCoords = world.GetWorldCoordsAtOffset(currentWorldCoords, IntVec2((int) stepX, 0));
            currentChunk = world.GetActiveChunk(currentWorldCoords.m_chunkCoords);
            if (!currentChunk)
            {
                return result; // Cannot continue, no-hit
            }

            if (totalRayLengthX >= result.m_raycast.m_maxDistance)
            {
                return result;
            }

            totalRayLength = totalRayLengthX;

            if (currentChunk->IsTileSolid(currentWorldCoords.m_localTileCoords))
            {
                result.m_blockingHit = true;
                result.m_hitNormal = Vec2(-stepX, 0.f);
                result.m_hitLocation = result.m_raycast.m_start + result.m_raycast.m_direction * totalRayLength;
                result.m_distance = totalRayLength;
                result.m_t = totalRayLength / raycast.m_maxDistance;
                return result;
            }

            totalRayLengthX += rayLengthPerStepX * world.m_worldSettings.m_tileWidth; // prepare for next iteration
        }
        else
        {
            currentWorldCoords = world.GetWorldCoordsAtOffset(currentWorldCoords, IntVec2(0, (int) stepY));
            currentChunk = world.GetActiveChunk(currentWorldCoords.m_chunkCoords);
            if (!currentChunk)
            {
                return result; // Cannot continue, no-hit
            }

            if (totalRayLengthY >= result.m_raycast.m_maxDistance)
            {
                return result;
            }

            totalRayLength = totalRayLengthY;

            if (currentChunk->IsTileSolid(currentWorldCoords.m_localTileCoords))
            {
                result.m_blockingHit = true;
                result.m_hitNormal = Vec2(0.f, -stepY);
                result.m_hitLocation = result.m_raycast.m_start + result.m_raycast.m_direction * totalRayLength;
                result.m_distance = totalRayLength;
                result.m_t = totalRayLength / raycast.m_maxDistance;
                return result;
            }

            totalRayLengthY += rayLengthPerStepY * world.m_worldSettings.m_tileWidth; // prepare for next iteration
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
        AddVertsForArrow2D(vbo.GetMutableVerts(), result.m_raycast.m_start, result.m_raycast.m_start + result.m_hitNormal, 0.05f, Rgba8::Orange);
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
