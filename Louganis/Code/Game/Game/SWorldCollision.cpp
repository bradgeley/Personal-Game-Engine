// Bradley Christensen - 2023
#include "SWorldCollision.h"
#include "CCollision.h"
#include "CTransform.h"
#include "CRender.h"
#include "Chunk.h"
#include "SCWorld.h"
#include "Engine/Math/GeometryUtils.h"
#include "WorldRaycast.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorldCollision::Startup()
{
    AddWriteDependencies<CTransform>();
    AddReadDependencies<CCollision, SCWorld, CRender>();
}



//----------------------------------------------------------------------------------------------------------------------
void SWorldCollision::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collStorage = g_ecs->GetArrayStorage<CCollision>();
    auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
    auto& scWorld = g_ecs->GetSingleton<SCWorld>();

    for (auto it = g_ecs->Iterate<CTransform, CCollision, CRender>(context); it.IsValid(); ++it)
    {
        CTransform& trans = transStorage[it.m_currentIndex];
        CCollision& coll = *collStorage.Get(it.m_currentIndex);
        CRender& render = *renderStorage.Get(it.m_currentIndex);

        // Pretty big assumption: renderPos is the position last frame, unadulturated by anything other than movement.
        Vec2& pos = trans.m_pos;
        Vec2& renderPos = render.m_pos;
        float& radius = coll.m_radius;

        WorldRaycast raycast;
        Vec2 posToLastFramePos = pos - renderPos;
        float distance = posToLastFramePos.GetLength() - coll.m_radius;
        if (distance > 0.f)
        {
            raycast.m_direction = posToLastFramePos / distance;
            raycast.m_start = render.m_pos;
            raycast.m_maxDistance = distance;
            raycast.m_queryWorldTiles = true;
            WorldRaycastResult result = Raycast(scWorld, raycast);
            if (result.m_blockingHit && !result.m_immediateHit)
            {
                Vec2 impactPointToFinalPos = pos - result.m_hitLocation;
                Vec2 impactPointToFinalPos_AlongNorm = impactPointToFinalPos.GetProjectedOntoNormal(result.m_hitNormal);
                pos -= impactPointToFinalPos_AlongNorm - (result.m_hitNormal * coll.m_radius);
            }
        }

        WorldCoords worldCoords = scWorld.GetWorldCoordsAtLocation(trans.m_pos);
        if (Chunk* chunk = scWorld.GetActiveChunk(worldCoords))
        {
            if (chunk->IsTileSolid(worldCoords.m_localTileCoords))
            {
                // can't push out if we are inside a wall
                continue;
            }

            constexpr int numNeighbors = 8;
            WorldCoords neighborCoords[numNeighbors] =
            {
                scWorld.GetWorldCoordsAtOffset(worldCoords, IntVec2(1, 0)),
                scWorld.GetWorldCoordsAtOffset(worldCoords, IntVec2(-1, 0)),
                scWorld.GetWorldCoordsAtOffset(worldCoords, IntVec2(0, 1)),
                scWorld.GetWorldCoordsAtOffset(worldCoords, IntVec2(0, -1)),
                scWorld.GetWorldCoordsAtOffset(worldCoords, IntVec2(1, 1)),
                scWorld.GetWorldCoordsAtOffset(worldCoords, IntVec2(-1, -1)),
                scWorld.GetWorldCoordsAtOffset(worldCoords, IntVec2(-1, 1)),
                scWorld.GetWorldCoordsAtOffset(worldCoords, IntVec2(1, -1))
            };

            for (int neighborIndex = 0; neighborIndex < numNeighbors; ++neighborIndex)
            {
                WorldCoords& neighborCoord = neighborCoords[neighborIndex];
                if (Chunk* neighborChunk = scWorld.GetActiveChunk(neighborCoord))
                {
                    if (neighborChunk->IsTileSolid(neighborCoord.m_localTileCoords))
                    {
                        AABB2 neighborTileBounds = scWorld.GetTileBounds(neighborCoord);
                        PushDiscOutOfAABB2D(pos, radius, neighborTileBounds);
                    }
                }
            }
        }
    }
}