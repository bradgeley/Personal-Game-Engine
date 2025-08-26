// Bradley Christensen - 2022-2025
#include "WorldRaycast.h"
#include "SCWorld.h"
#include "Chunk.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Performance/ScopedTimer.h"



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

    float stepX = MathUtils::SignF(raycast.m_direction.x);
    float stepY = MathUtils::SignF(raycast.m_direction.y);
    
    float toLeadingEdgeX = stepX < 0.f ? tileMinsToRayStart.x : world.m_worldSettings.m_tileWidth - tileMinsToRayStart.x;
    float toLeadingEdgeY = stepY < 0.f ? tileMinsToRayStart.y : world.m_worldSettings.m_tileWidth - tileMinsToRayStart.y;

    float rayLengthPerStepX = MathUtils::AbsF(1.f / raycast.m_direction.x);
    float rayLengthPerStepY = MathUtils::AbsF(1.f / raycast.m_direction.y);

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
WorldDiscCastResult DiscCast(SCWorld const& world, WorldDiscCast const& discCast)
{
    WorldDiscCastResult result;
    result.m_discCast = discCast;
    result.m_hitLocation = discCast.m_start;
    Vec2 const discCastEndPoint = discCast.m_start + discCast.m_direction * discCast.m_maxDistance;
    result.m_newDiscCenter = discCastEndPoint;

    if (discCast.m_direction == Vec2::ZeroVector)
    {
        return result;
    }

    result.m_hitLocation = discCast.m_start + discCast.m_direction * discCast.m_maxDistance;

    // Check for initial hit
    float nearestDistSquared = FLT_MAX;
    world.ForEachSolidWorldCoordsOverlappingCapsule(discCast.m_start, discCast.m_start, discCast.m_discRadius, [&world, &discCast, &result, &nearestDistSquared](WorldCoords& coords)
    {
        AABB2 tileBounds = world.GetTileBounds(coords);
        Vec2 nearestPoint = tileBounds.GetNearestPoint(discCast.m_start);
    
        float distSquared = nearestPoint.GetDistanceSquaredTo(discCast.m_start);
        if (distSquared < nearestDistSquared)
        {
            nearestDistSquared = distSquared;
    
            result.m_blockingHit = true;
            result.m_immediateHit = true;
            result.m_newDiscCenter = discCast.m_start;
            result.m_hitLocation = nearestPoint;
            result.m_hitNormal = (result.m_newDiscCenter - result.m_hitLocation).GetNormalized();
            result.m_t = 0.f;
            result.m_distance = 0.f;
        }
        return true;
    });

    // Sweep against all the tiles in the path
    world.ForEachSolidWorldCoordsOverlappingCapsule(discCast.m_start, discCastEndPoint, discCast.m_discRadius, [&world, &discCast, &result, &discCastEndPoint](WorldCoords& coords)
    {
        // Line vs. expanded tile bounds for sweep test
        AABB2 tileBounds = world.GetTileBounds(coords);
        AABB2 expandedTileBounds = tileBounds.GetExpandedBy(discCast.m_discRadius);

        float tOfFirstIntersection;
        if (GeometryUtils::GetFirstLineAABBIntersection(discCast.m_start, discCastEndPoint, expandedTileBounds, tOfFirstIntersection))
        {
            Vec2 newDiscCenter = discCast.m_start + discCast.m_direction * discCast.m_maxDistance * tOfFirstIntersection;
            Vec2 nearestPoint = tileBounds.GetNearestPoint(newDiscCenter);
            float distSquared = nearestPoint.GetDistanceSquaredTo(newDiscCenter);

            float radiusSquared = discCast.m_discRadius * discCast.m_discRadius;
            if (distSquared > radiusSquared)
            {
                // Do Corner hit detection
                Vec2 corners[4] = { tileBounds.mins, tileBounds.maxs, tileBounds.GetBottomRight(), tileBounds.GetTopLeft() };

                float smallestCornerT = FLT_MAX;
                int cornerHitIndex = -1;
                for (int cornerIndex = 0; cornerIndex < 4; ++cornerIndex)
                {
                    Vec2 const& corner = corners[cornerIndex];
                    float cornerT;
                    if (GeometryUtils::SweepDiscVsPoint(discCast.m_start, discCastEndPoint, discCast.m_discRadius, corner, cornerT))
                    {
                        if (cornerT < smallestCornerT)
                        {
                            cornerHitIndex = cornerIndex;
                            smallestCornerT = cornerT;
                        }
                    }
                }

                if (smallestCornerT >= 0.f 
                    && smallestCornerT <= 1.f
                    && smallestCornerT <= result.m_t)
                {
                    // Corner hit
                    result.m_blockingHit = true;
                    result.m_t = smallestCornerT;
                    result.m_distance = discCast.m_maxDistance * result.m_t;
                    result.m_newDiscCenter = discCast.m_start + discCast.m_direction * result.m_distance;
                    result.m_hitLocation = corners[cornerHitIndex];
                    result.m_hitNormal = (result.m_newDiscCenter - result.m_hitLocation).GetNormalized();
                }

            }
            else if (tOfFirstIntersection <= result.m_t)
            {
                // Side wall hit
                result.m_blockingHit = true;
                result.m_t = tOfFirstIntersection;
                result.m_distance = discCast.m_maxDistance * result.m_t;
                result.m_newDiscCenter = newDiscCenter;
                result.m_hitLocation = tileBounds.GetNearestPoint(result.m_newDiscCenter);
                result.m_hitNormal = (result.m_newDiscCenter - result.m_hitLocation).GetNormalized();
            }
        }
        return true;
    });

    return result;
}



//----------------------------------------------------------------------------------------------------------------------
void DebugDrawRaycast(WorldRaycastResult const& result)
{
    VertexBufferID id = g_renderer->MakeVertexBuffer();
    VertexBuffer& vbo = *g_renderer->GetVertexBuffer(id);
    vbo.Initialize(1024);

    VertexUtils::AddVertsForDisc2D(vbo, result.m_raycast.m_start, 0.1f, 16, Rgba8::Yellow);
    VertexUtils::AddVertsForDisc2D(vbo, result.m_hitLocation, 0.1f, 16, Rgba8::Yellow);

    if (result.m_immediateHit)
    {
        VertexUtils::AddVertsForArrow2D(vbo, result.m_raycast.m_start, result.m_hitLocation, 0.05f, Rgba8::Gray);
        VertexUtils::AddVertsForArrow2D(vbo, result.m_raycast.m_start, result.m_raycast.m_start + result.m_hitNormal, 0.05f, Rgba8::Orange);
    }
    else if (result.m_blockingHit)
    {
        VertexUtils::AddVertsForArrow2D(vbo, result.m_raycast.m_start, result.m_hitLocation, 0.05f, Rgba8::Red);
        VertexUtils::AddVertsForArrow2D(vbo, result.m_hitLocation, result.m_hitLocation + result.m_hitNormal, 0.05f, Rgba8::Red);
    }
    else
    {
        VertexUtils::AddVertsForArrow2D(vbo, result.m_raycast.m_start, result.m_hitLocation, 0.05f, Rgba8::Yellow);
    }

    g_renderer->BindTexture(nullptr);
    g_renderer->BindShader(nullptr);
    g_renderer->DrawVertexBuffer(vbo);

    g_renderer->ReleaseVertexBuffer(id);
}



//----------------------------------------------------------------------------------------------------------------------
void AddVertsForRaycast(VertexBuffer& vbo, WorldRaycastResult const& result, float scaleMultiplier /*= 1.f*/)
{
    static int numTrianglesPerDisc = 16;
    static float pointRadius = 0.1f;
    static float arrowThickness = 0.05f;

    VertexUtils::AddVertsForDisc2D(vbo, result.m_raycast.m_start, pointRadius * scaleMultiplier, numTrianglesPerDisc, Rgba8::Yellow);
    VertexUtils::AddVertsForDisc2D(vbo, result.m_hitLocation, pointRadius * scaleMultiplier, numTrianglesPerDisc, Rgba8::Yellow);

    if (result.m_immediateHit)
    {
        VertexUtils::AddVertsForArrow2D(vbo, result.m_raycast.m_start, result.m_hitLocation, arrowThickness * scaleMultiplier, Rgba8::Gray);
        VertexUtils::AddVertsForArrow2D(vbo, result.m_raycast.m_start, result.m_raycast.m_start + result.m_hitNormal, arrowThickness * scaleMultiplier, Rgba8::Orange);
    }
    else if (result.m_blockingHit)
    {
        VertexUtils::AddVertsForArrow2D(vbo, result.m_raycast.m_start, result.m_hitLocation, arrowThickness * scaleMultiplier, Rgba8::Red);
        VertexUtils::AddVertsForArrow2D(vbo, result.m_hitLocation, result.m_hitLocation + result.m_hitNormal, arrowThickness * scaleMultiplier, Rgba8::Red);
    }
    else
    {
        VertexUtils::AddVertsForArrow2D(vbo, result.m_raycast.m_start, result.m_hitLocation, arrowThickness * scaleMultiplier, Rgba8::Yellow);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AddVertsForDiscCast(VertexBuffer& vbo, WorldDiscCastResult const& result, float scaleMultiplier)
{
    static int numTrianglesPerDisc = 16;
    static float pointRadius = 0.1f;
    static float arrowThickness = 0.05f;

    VertexUtils::AddVertsForCapsule2D(vbo, result.m_discCast.m_start, result.m_discCast.m_start + result.m_discCast.m_direction * result.m_discCast.m_maxDistance, result.m_discCast.m_discRadius, Rgba8(0, 0, 0, 25));
    VertexUtils::AddVertsForCapsule2D(vbo, result.m_discCast.m_start, result.m_discCast.m_start + result.m_discCast.m_direction * result.m_discCast.m_maxDistance * result.m_t, result.m_discCast.m_discRadius, Rgba8(0, 0, 0, 127));

    VertexUtils::AddVertsForArrow2D(vbo, result.m_discCast.m_start, result.m_newDiscCenter, arrowThickness * scaleMultiplier, Rgba8::Yellow);
    VertexUtils::AddVertsForDisc2D(vbo, result.m_newDiscCenter, pointRadius * scaleMultiplier, numTrianglesPerDisc, Rgba8::Yellow);

    if (result.m_blockingHit)
    {
        VertexUtils::AddVertsForDisc2D(vbo, result.m_discCast.m_start, pointRadius * scaleMultiplier, numTrianglesPerDisc, Rgba8::Yellow);
        VertexUtils::AddVertsForDisc2D(vbo, result.m_hitLocation, pointRadius * scaleMultiplier, numTrianglesPerDisc, Rgba8::Yellow);

        VertexUtils::AddVertsForArrow2D(vbo, result.m_hitLocation, result.m_hitLocation + result.m_hitNormal, arrowThickness * scaleMultiplier, Rgba8::Red);
    }
}
