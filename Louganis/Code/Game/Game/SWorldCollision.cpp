// Bradley Christensen - 2023
#include "SWorldCollision.h"
#include "CCollision.h"
#include "CTransform.h"
#include "CRender.h"
#include "Chunk.h"
#include "SCDebug.h"
#include "SCWorld.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Math/GeometryUtils.h"
#include "WorldRaycast.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorldCollision::Startup()
{
    AddWriteDependencies<CTransform>();
    AddReadDependencies<CCollision, SCWorld>();
    AddWriteDependencies<SCDebug>();
}



//----------------------------------------------------------------------------------------------------------------------
void SWorldCollision::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collStorage = g_ecs->GetArrayStorage<CCollision>();
    auto& scWorld = g_ecs->GetSingleton<SCWorld>();

    for (auto it = g_ecs->Iterate<CTransform, CCollision, CRender>(context); it.IsValid(); ++it)
    {
        CTransform& trans = transStorage[it.m_currentIndex];
        CCollision& coll = *collStorage.Get(it.m_currentIndex);

        Vec2& pos = trans.m_pos;
        float& radius = coll.m_radius;

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
                        GeometryUtils::PushDiscOutOfAABB2D(pos, radius + scWorld.m_worldSettings.m_entityWallBuffer, neighborTileBounds);
                    }
                }
            }
        }
    }
}