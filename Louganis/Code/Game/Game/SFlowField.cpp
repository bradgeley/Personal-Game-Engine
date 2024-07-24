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



IntVec2 neighborOffsets[4] = { IntVec2(1,  0),
                               IntVec2(-1,  0),
                               IntVec2(0,  1),
                               IntVec2(0, -1) };



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
    Chunk* playerChunk = world.GetActiveChunkAtLocation(firstPlayerLocation);
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

    // Create flow field chunks for all chunks in the world
    //for (auto it : world.m_activeChunks)
    //{
    //    Chunk* chunk = it.second;
    //    FlowFieldChunk* flowFieldChunk = scFlowField.GetActiveChunk(chunk->m_chunkCoords);
    //    if (!flowFieldChunk)
    //    {
    //        flowFieldChunk = new FlowFieldChunk(chunk);
    //        scFlowField.m_activeFlowFieldChunks.emplace(chunk->m_chunkCoords, flowFieldChunk);
    //    }
    //}

    // Destroy flow field chunks that no longer have a valid chunk
    //std::vector<IntVec2> coordsToRemove;
    //for (auto it : scFlowField.m_activeFlowFieldChunks)
    //{
    //    FlowFieldChunk*& flowFieldChunk = it.second;
    //    if (!world.GetActiveChunk(flowFieldChunk->m_chunkCoords))
    //    {
    //        coordsToRemove.push_back(flowFieldChunk->m_chunkCoords);
    //    }
    //}
    //for (auto& coords : coordsToRemove)
    //{
    //    scFlowField.m_activeFlowFieldChunks.erase(coords);
    //}

    FlowFieldChunk* playerFlowFieldChunk = scFlowField.GetActiveChunk(playerChunk->m_chunkCoords);
    if (!playerFlowFieldChunk)
    {
        playerFlowFieldChunk = new FlowFieldChunk(playerChunk);
        scFlowField.m_activeFlowFieldChunks.emplace(playerChunk->m_chunkCoords, playerFlowFieldChunk);
    }

    GenerateChunk(playerFlowFieldChunk, playerLocalTileCoords);

    FlowFieldChunk* eastOfPlayerFlowFieldChunk = scFlowField.GetActiveChunk(playerChunk->m_chunkCoords + IntVec2(1, 0));
    Chunk* eastOfPlayerChunk = world.GetActiveChunk(playerChunk->m_chunkCoords + IntVec2(1, 0));
    if (!eastOfPlayerFlowFieldChunk)
    {
        eastOfPlayerFlowFieldChunk = new FlowFieldChunk(eastOfPlayerChunk);
        scFlowField.m_activeFlowFieldChunks.emplace(eastOfPlayerChunk->m_chunkCoords, eastOfPlayerFlowFieldChunk);
    }

    GenerateChunk(eastOfPlayerFlowFieldChunk, IntVec2(-1, -1));
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::GenerateChunk(FlowFieldChunk* chunk,  IntVec2 const& seedLocalTileCoords)
{
    GenerateCostField(chunk);
    SeedDistanceField(chunk, seedLocalTileCoords);
    GenerateDistanceField(chunk);
    GenerateGradient(chunk);
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::GenerateCostField(FlowFieldChunk* chunk)
{
    ScopedTimer timer("Generate Cost Field");

    // Generate Cost Field
    for (int y = 0; y < chunk->m_costField.GetHeight(); ++y)
    {
        for (int x = 0; x < chunk->m_costField.GetWidth(); ++x)
        {
            IntVec2 localTileCoords = IntVec2(x, y);
            if (chunk->m_chunk->IsTileSolid(localTileCoords))
            {
                chunk->m_costField.Set(localTileCoords, 255);
            }
            else
            {
                chunk->m_costField.Set(localTileCoords, 1);
            }
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::SeedDistanceField(FlowFieldChunk* chunk, IntVec2 const& seedLocalTileCoords)
{
    chunk->m_distanceField.SetAll(999.f);
    chunk->m_consideredNodes.SetAll(false);
    if (chunk->m_distanceField.IsValidCoords(seedLocalTileCoords))
    {
        chunk->m_distanceField.Set(seedLocalTileCoords, 0.f);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::GenerateDistanceField(FlowFieldChunk* chunk)
{
    ScopedTimer timer("Distance Field (FMM)");
    SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();

    for (int i = 0; i < chunk->m_distanceField.Count(); ++i)
    {
        IntVec2 currentLocalTileCoords = chunk->m_distanceField.GetCoordsForIndex(i);
        if (chunk->m_distanceField.Get(i) == 0.f)
        {
            scFlowField.m_openList.push({ chunk->m_chunkCoords, currentLocalTileCoords, 0.f });
        }

        if (!chunk->m_distanceField.IsOnEdge(currentLocalTileCoords))
        {
            continue;
        }

        for (IntVec2 const& neighborOffset : neighborOffsets)
        {
            IntVec2 neighborChunkOffset;
            IntVec2 neighborLocalTileCoords = world.GetLocalTileCoordsAtOffset(currentLocalTileCoords, neighborOffset, neighborChunkOffset);
            IntVec2 neighborChunkCoords = chunk->m_chunkCoords + neighborChunkOffset;
            FlowFieldChunk* neighborChunk = scFlowField.GetActiveChunk(neighborChunkCoords);
            if (!neighborChunk || neighborChunk == chunk)
            {
                continue;
            }
            if (neighborChunk->m_chunk->IsTileSolid(neighborLocalTileCoords))
            {
                continue;
            }

            neighborChunk->m_consideredNodes.Set(neighborLocalTileCoords, false);
            scFlowField.m_openList.push({ neighborChunk->m_chunkCoords, neighborLocalTileCoords, neighborChunk->m_distanceField.Get(neighborLocalTileCoords) });
        }
    }

    while (!scFlowField.m_openList.empty())
    {
        ChunkAndTile chunkAndTile = scFlowField.m_openList.top();
        IntVec2 const& currentChunkCoords = chunkAndTile.m_chunkCoords;
        IntVec2 const& currentLocalTileCoords = chunkAndTile.m_localTileCoords;
        scFlowField.m_openList.pop();

        FlowFieldChunk* currentChunk = scFlowField.m_activeFlowFieldChunks.at(currentChunkCoords);
        currentChunk->m_consideredNodes.Set(currentLocalTileCoords, true);

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
                float nofnDistance = nofnChunk->m_distanceField.Get(nofnLocalTileCoords);
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

            if (calculatedNeighborDistance < neighborChunk->m_distanceField.Get(neighborLocalTileCoords))
            {
                neighborChunk->m_distanceField.Set(neighborLocalTileCoords, calculatedNeighborDistance);
                scFlowField.m_openList.push({ neighborChunkCoords, neighborLocalTileCoords, calculatedNeighborDistance });
            }
        }
    }
}

void SFlowField::GenerateGradient(FlowFieldChunk* chunk)
{
    ScopedTimer timer("Generate Gradient");
    SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();

    for (int i = 0; i < chunk->m_distanceField.Count(); ++i)
    {
        IntVec2 currentLocalTileCoords = chunk->m_distanceField.GetCoordsForIndex(i);
        if (chunk->m_distanceField.Get(i) == 0.f)
        {
            IntVec2 localTileCoords = chunk->m_distanceField.GetCoordsForIndex(i);
            scFlowField.m_openList.push({ chunk->m_chunkCoords, localTileCoords, 0.f });
        }
        if (!chunk->m_distanceField.IsOnEdge(currentLocalTileCoords))
        {
            continue;
        }

        for (IntVec2 const& neighborOffset : neighborOffsets)
        {
            IntVec2 neighborChunkOffset;
            IntVec2 neighborLocalTileCoords = world.GetLocalTileCoordsAtOffset(currentLocalTileCoords, neighborOffset, neighborChunkOffset);
            IntVec2 neighborChunkCoords = chunk->m_chunkCoords + neighborChunkOffset;
            FlowFieldChunk* neighborChunk = scFlowField.GetActiveChunk(neighborChunkCoords);
            if (!neighborChunk || neighborChunk == chunk)
            {
                continue;
            }
            if (neighborChunk->m_chunk->IsTileSolid(neighborLocalTileCoords))
            {
                continue;
            }

            neighborChunk->m_consideredNodes.Set(neighborLocalTileCoords, false);
            scFlowField.m_openList.push({ neighborChunk->m_chunkCoords, neighborLocalTileCoords, neighborChunk->m_distanceField.Get(neighborLocalTileCoords) });
        }
    }

    // Reset all considered nodes on tiles back to false - necessary before every flow field creation
    for (auto it : scFlowField.m_activeFlowFieldChunks)
    {
        it.second->m_consideredNodes.SetAll(false);
    }

    while (!scFlowField.m_openList.empty())
    {
        ChunkAndTile chunkAndTile = scFlowField.m_openList.top();
        IntVec2 const& currentChunkCoords = chunkAndTile.m_chunkCoords;
        IntVec2 const& currentLocalTileCoords = chunkAndTile.m_localTileCoords;
        scFlowField.m_openList.pop();

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

        float currentTileDistance = (float) currentChunk->m_distanceField.Get(currentLocalTileCoords);

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

            float currentNeighborDistance = (float) neighborChunk->m_distanceField.Get(neighborLocalTileCoords);
            float dDist = currentTileDistance - currentNeighborDistance;

            if (neighborOffset.x != 0.f)
            {
                gradient.x += dDist * Sign(neighborOffset.x);
                numNeighborsConsideredInDir.x += 1.f;
            }
            else
            {
                gradient.y += dDist * Sign(neighborOffset.y);
                numNeighborsConsideredInDir.y += 1.f;
            }

            if (neighborChunk->m_consideredNodes.Get(neighborLocalTileCoords) == false)
            {
                scFlowField.m_openList.push({ neighborChunkCoords, neighborLocalTileCoords, currentNeighborDistance });
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
