// Bradley Christensen - 2023
#include "SFlowField.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "SCFlowField.h"
#include "CPlayerController.h"
#include "FlowFieldChunk.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Chunk.h"
#include <queue>



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::Startup()
{
    AddReadDependencies<CTransform, SCWorld>();
    AddWriteDependencies<SCFlowField>();
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::Run(SystemContext const& context)
{
    Vec2 firstPlayerLocation;

    SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    for (auto it = g_ecs->Iterate<CTransform, CPlayerController>(context); it.IsValid(); ++it)
    {
        firstPlayerLocation = transStorage.Get(it)->m_pos;
    }

    SCWorld& world = g_ecs->GetSingleton<SCWorld>();
    Chunk* chunk = world.GetActiveChunkAtLocation(firstPlayerLocation);
    IntVec2 playerLocalTileCoords = world.GetLocalTileCoordsAtLocation(firstPlayerLocation);
    IntVec2 playerWorldTileCoords = world.GetWorldTileCoordsAtLocation(firstPlayerLocation);
    
    bool playerMoved = false;
    if (playerWorldTileCoords != scFlowField.m_lastKnownPlayerLocation)
    {
        scFlowField.m_lastKnownPlayerLocation = playerWorldTileCoords;
        playerMoved = true;
    }

    if (playerMoved)
    {
        //scFlowField.Reset();
    }
    else
    {
        return;
    }


    FlowFieldChunk* flowFieldChunk = scFlowField.GetActiveChunk(chunk->m_chunkCoords);
    if (!flowFieldChunk)
    {
        flowFieldChunk = new FlowFieldChunk(chunk);
        scFlowField.m_activeFlowFieldChunks.emplace(chunk->m_chunkCoords, flowFieldChunk);
    }

    {
        ScopedTimer timer("Generate Cost Field");
        for (auto chunkIt : scFlowField.m_activeFlowFieldChunks)
        {
            auto currentFlowFieldChunk = chunkIt.second;

            // Generate Cost Field
            for (int y = 0; y < currentFlowFieldChunk->m_costField.GetHeight(); ++y)
            {
                for (int x = 0; x < currentFlowFieldChunk->m_costField.GetWidth(); ++x)
                {
                    IntVec2 localTileCoords = IntVec2(x, y);
                    if (currentFlowFieldChunk->m_chunk->IsTileSolid(localTileCoords))
                    {
                        currentFlowFieldChunk->m_costField.Set(localTileCoords, 255);
                    }
                    else
                    {
                        currentFlowFieldChunk->m_costField.Set(localTileCoords, 1);
                    }
                }
            }
        }
    }


    struct ChunkAndTile
    {
        IntVec2 m_chunkCoords;
        IntVec2 m_localTileCoords;
        float m_distance = 0.f;

        bool operator<(ChunkAndTile const& rhs) const { return m_distance > rhs.m_distance; }
    };

    std::priority_queue<ChunkAndTile> openList; // todo: use pqueue

    // Generate generic Dijkstra map

    //flowFieldChunk->m_distanceField.Set(playerLocalTileCoords, 0);
    //openList.push({ chunk->m_chunkCoords, playerLocalTileCoords });
    //
    //while (!openList.empty())
    //{
    //    ChunkAndTile chunkAndTile = openList.front();
    //    IntVec2 const& currentChunkCoords = chunkAndTile.m_chunkCoords;
    //    IntVec2 const& currentLocalTileCoords = chunkAndTile.m_localTileCoords;
    //    openList.pop();
    //
    //    FlowFieldChunk* currentChunk = scFlowField.m_activeFlowFieldChunks.at(currentChunkCoords);
    //    int currentTileDistance = currentChunk->m_distanceField.Get(currentLocalTileCoords);
    //
    //    IntVec2 neighborOffsets[4] = { IntVec2( 1,  0),
    //                                   IntVec2(-1,  0),
    //                                   IntVec2( 0,  1),
    //                                   IntVec2( 0, -1)  };
    //
    //    for (IntVec2 const& neighborOffset : neighborOffsets)
    //    {
    //        IntVec2 neighborChunkOffset;
    //        IntVec2 neighborLocalTileCoords = world.GetLocalTileCoordsAtOffset(currentLocalTileCoords, neighborOffset, neighborChunkOffset);
    //        IntVec2 neighborChunkCoords = currentChunkCoords + neighborChunkOffset;
    //        FlowFieldChunk* neighborChunk = scFlowField.GetActiveChunk(neighborChunkCoords);
    //        if (!neighborChunk)
    //        {
    //            continue;
    //        }
    //        if (neighborChunk->m_chunk->IsTileSolid(neighborLocalTileCoords))
    //        {
    //            continue;
    //        }
    //
    //        int neighborCost = (int) neighborChunk->m_costField.Get(neighborLocalTileCoords);
    //        int currentNeighborDistance = neighborChunk->m_distanceField.Get(neighborLocalTileCoords);
    //        int calculatedNeighborDistance = currentTileDistance + neighborCost;
    //        if (calculatedNeighborDistance < currentNeighborDistance)
    //        {
    //            neighborChunk->m_distanceField.Set(neighborLocalTileCoords, calculatedNeighborDistance);
    //            openList.push({ neighborChunkCoords, neighborLocalTileCoords });
    //        }
    //    }
    //}

    // Generate Flow Field using Fast Marching Method to calculate distance using the Eikonal equation
    {
        ScopedTimer timer("Distance Field FMM");
        for (auto chunkIt : scFlowField.m_activeFlowFieldChunks)
        {
            auto currentFlowFieldChunk = chunkIt.second;
            currentFlowFieldChunk->m_flowField.SetAll(999.f);
            currentFlowFieldChunk->m_consideredNodes.SetAll(false);
        }

        flowFieldChunk->m_flowField.Set(playerLocalTileCoords, 0.f);
        openList.push({ chunk->m_chunkCoords, playerLocalTileCoords, 0.f });

        while (!openList.empty())
        {
            ChunkAndTile chunkAndTile = openList.top();
            IntVec2 const& currentChunkCoords = chunkAndTile.m_chunkCoords;
            IntVec2 const& currentLocalTileCoords = chunkAndTile.m_localTileCoords;
            openList.pop();

            FlowFieldChunk* currentChunk = scFlowField.m_activeFlowFieldChunks.at(currentChunkCoords);
            currentChunk->m_consideredNodes.Set(currentLocalTileCoords, true);

            IntVec2 neighborOffsets[4] = { IntVec2(1,  0),
                                           IntVec2(-1,  0),
                                           IntVec2(0,  1),
                                           IntVec2(0, -1) };

            for (IntVec2 const& neighborOffset : neighborOffsets)
            {
                IntVec2 neighborChunkOffset;
                IntVec2 neighborLocalTileCoords = world.GetLocalTileCoordsAtOffset(currentLocalTileCoords, neighborOffset, neighborChunkOffset);
                IntVec2 neighborChunkCoords = currentChunkCoords + neighborChunkOffset;
                FlowFieldChunk* neighborChunk = scFlowField.GetActiveChunk(neighborChunkCoords);
                if (!neighborChunk)
                {
                    continue;
                }
                if (neighborChunk->m_consideredNodes.Get(neighborLocalTileCoords) == true)
                {
                    continue;
                }
                if (neighborChunk->m_chunk->IsTileSolid(neighborLocalTileCoords))
                {
                    continue;
                }

                int neighborCost = (int) neighborChunk->m_costField.Get(neighborLocalTileCoords);

                float dx = FLT_MAX; // dx = min( neighborWestDistance, neighborEastDistance );
                float dy = FLT_MAX; // dy = min( neighborNorthDistance, neighborSouthDistance );

                // nofn = neighbor of neighbor
                for (IntVec2 const& nofnOffset : neighborOffsets)
                {
                    IntVec2 nofnChunkOffset;
                    IntVec2 nofnLocalTileCoords = world.GetLocalTileCoordsAtOffset(neighborLocalTileCoords, nofnOffset, nofnChunkOffset);
                    IntVec2 nofnChunkCoords = neighborChunkCoords + nofnChunkOffset;
                    FlowFieldChunk* nofnChunk = scFlowField.GetActiveChunk(nofnChunkCoords);
                    if (!nofnChunk)
                    {
                        continue;
                    }
                    if (nofnChunk->m_chunk->IsTileSolid(nofnLocalTileCoords))
                    {
                        continue;
                    }
                    float nofnDistance = nofnChunk->m_flowField.Get(nofnLocalTileCoords);
                    if (nofnOffset.x != 0 && dx > nofnDistance)
                    {
                        dx = nofnDistance; // get the minimum of east/west distances
                    }
                    else if (nofnOffset.y != 0 && dy > nofnDistance)
                    {
                        dy = nofnDistance; // get the minimum of north/south distances
                    }
                }

                // Delta = 2 * neighborCost - (dx-dy)^2;
                // if Delta >= 0
                //     D(j) = (dx + dy + sqrt(Delta)) / 2;
                // else
                //     D(j) = min(dx + W(j), dy + W(j));
                // end

                float delta = 2 * neighborCost - PowF(dx - dy, 2);
                float calculatedNeighborDistance;
                if (delta >= 0)
                {
                    calculatedNeighborDistance = (dx + dy + SqrtF(delta)) / 2.f;
                }
                else
                {
                    calculatedNeighborDistance = MinF(dx + neighborCost, dy + neighborCost);
                }

                if (calculatedNeighborDistance < neighborChunk->m_flowField.Get(neighborLocalTileCoords))
                {
                    neighborChunk->m_flowField.Set(neighborLocalTileCoords, calculatedNeighborDistance);
                    openList.push({ neighborChunkCoords, neighborLocalTileCoords, calculatedNeighborDistance });
                }
            }
        }
    }

    // Generate gradient of each point on distance field
    {
        ScopedTimer timer("Generate Gradient");
        openList.push({ chunk->m_chunkCoords, playerLocalTileCoords, 0.f });
    
        // Reset all considered nodes on tiles back to false - necessary before every flow field creation
        for (auto it : scFlowField.m_activeFlowFieldChunks)
        {
            it.second->m_consideredNodes.SetAll(false);
        }
    
        while (!openList.empty())
        {
            ChunkAndTile chunkAndTile = openList.top();
            IntVec2 const& currentChunkCoords = chunkAndTile.m_chunkCoords;
            IntVec2 const& currentLocalTileCoords = chunkAndTile.m_localTileCoords;
            openList.pop();
    
            FlowFieldChunk* currentChunk = scFlowField.m_activeFlowFieldChunks.at(currentChunkCoords);
            if (currentChunk->m_consideredNodes.Get(currentLocalTileCoords))
            {
                continue;
            }
            if (currentChunk->m_chunk->IsTileSolid(currentLocalTileCoords))
            {
                continue;
            }

            currentChunk->m_consideredNodes.Set(currentLocalTileCoords, true);
    
            float currentTileDistance = (float) currentChunk->m_flowField.Get(currentLocalTileCoords);
    
            IntVec2 neighborOffsets[4] = { IntVec2(1,  0),
                                           IntVec2(-1,  0),
                                           IntVec2(0,  1),
                                           IntVec2(0, -1) };
    
            Vec2 gradient = Vec2::ZeroVector;
            Vec2 numNeighborsConsideredInDir; // if we reached a tile edge, there's only 1 neighbor to look at for the tile, so divide by 1 instead of 2
            for (IntVec2 const& neighborOffset : neighborOffsets)
            {
                IntVec2 neighborChunkOffset;
                IntVec2 neighborLocalTileCoords = world.GetLocalTileCoordsAtOffset(currentLocalTileCoords, neighborOffset, neighborChunkOffset);
                IntVec2 neighborChunkCoords = currentChunkCoords + neighborChunkOffset;
                FlowFieldChunk* neighborChunk = scFlowField.GetActiveChunk(neighborChunkCoords);
                if (!neighborChunk)
                {
                    continue;
                }
                if (neighborChunk->m_chunk->IsTileSolid(neighborLocalTileCoords))
                {
                    continue;
                }

                float currentNeighborDistance = (float) neighborChunk->m_flowField.Get(neighborLocalTileCoords);
                float dDist = currentTileDistance - currentNeighborDistance;
    
                if (neighborOffset.x != 0.f)
                {
                    gradient.x += dDist * SignF(neighborOffset.x);
                    numNeighborsConsideredInDir.x += 1.f;
                }
                else
                {
                    gradient.y += dDist * SignF(neighborOffset.y);
                    numNeighborsConsideredInDir.y += 1.f;
                }

                if (neighborChunk->m_consideredNodes.Get(neighborLocalTileCoords) == false)
                {
                    openList.push({ neighborChunkCoords, neighborLocalTileCoords, currentNeighborDistance });
                }
            }

            // Since we're calculating the gradient centrally (on both sides), divide by 2 (or 1 if only 1 tile was looked at bc of edges)
            if (numNeighborsConsideredInDir.x != 0.f)
            {
                gradient.x /= numNeighborsConsideredInDir.x;
            }
            if (numNeighborsConsideredInDir.y != 0.f)
            {
                gradient.y /= numNeighborsConsideredInDir.y;
            }

            //gradient.Normalize();
            currentChunk->m_gradient.Set(currentLocalTileCoords, gradient);
        }
    }
}