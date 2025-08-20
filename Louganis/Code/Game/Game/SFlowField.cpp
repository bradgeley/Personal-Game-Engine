// Bradley Christensen - 2023
#include "SFlowField.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "SCFlowField.h"
#include "CPlayerController.h"
#include "FlowFieldChunk.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Debug/DevConsole.h"
#include "Chunk.h"
#include <queue>



//----------------------------------------------------------------------------------------------------------------------
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
    FlowField& flowField = scFlowField.m_toPlayerFlowField;
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto it = g_ecs->Iterate<CTransform, CPlayerController>(context);
    if (it.IsValid())
    {
        firstPlayerLocation = transStorage.Get(it)->m_pos;
    }

    SCWorld& world = g_ecs->GetSingleton<SCWorld>();
    WorldCoords playerWorldCoords = world.GetWorldCoordsAtLocation(firstPlayerLocation);
    
    bool playerChangedCoords = false;
    if (playerWorldCoords != scFlowField.m_lastKnownPlayerWorldCoords)
    {
        scFlowField.m_lastKnownPlayerWorldCoords = playerWorldCoords;
        playerChangedCoords = true;
    }

    DestroyStaleFlowFieldChunks(firstPlayerLocation);
    int numCreated = CreateMissingFlowFieldChunks(firstPlayerLocation);

    if (numCreated > 0 || playerChangedCoords)
    {
        flowField.SoftReset();
        GenerateFlow(flowField, playerWorldCoords);
    }
}


//----------------------------------------------------------------------------------------------------------------------
void SFlowField::Shutdown()
{
    SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();
    FlowField& flowField = scFlowField.m_toPlayerFlowField;

    flowField.HardReset();
}


//----------------------------------------------------------------------------------------------------------------------
int SFlowField::CreateMissingFlowFieldChunks(Vec2 const& anchorLocation)
{
    SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();
    FlowField& flowField = scFlowField.m_toPlayerFlowField;

    int numCreated = 0;

    float flowFieldGenerationRadius = world.m_worldSettings.m_flowFieldGenerationRadius;
    float flowFieldGenerationRadiusSquared = flowFieldGenerationRadius * flowFieldGenerationRadius;

    AABB2 flowFieldBounds = AABB2(anchorLocation - Vec2(flowFieldGenerationRadius, flowFieldGenerationRadius), anchorLocation + Vec2(flowFieldGenerationRadius, flowFieldGenerationRadius));
    world.ForEachChunkOverlappingAABB(flowFieldBounds, [&flowField, &world, flowFieldGenerationRadiusSquared, &numCreated, &anchorLocation](Chunk& chunk)
    {
        FlowFieldChunk* flowFieldChunk = flowField.GetActiveChunk(chunk.m_chunkCoords);
        if (!flowFieldChunk)
        {
            float distanceSquared = chunk.m_chunkBounds.GetCenter().GetDistanceSquaredTo(anchorLocation);
            
            if (distanceSquared < flowFieldGenerationRadiusSquared)
            {
                flowFieldChunk = new FlowFieldChunk(&chunk, &world);
                flowFieldChunk->GenerateCostField();
                flowField.m_activeFlowFieldChunks.emplace(chunk.m_chunkCoords, flowFieldChunk);
                numCreated++;
            }
        }
        return true;
    });

    return numCreated;
}


//----------------------------------------------------------------------------------------------------------------------
int SFlowField::DestroyStaleFlowFieldChunks(Vec2 const& anchorLocation)
{
    SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();
    FlowField& flowField = scFlowField.m_toPlayerFlowField;

    float flowFieldGenerationRadiusSquared = world.m_worldSettings.m_flowFieldGenerationRadius * world.m_worldSettings.m_flowFieldGenerationRadius;

    // Destroy flow field chunks that no longer have a valid chunk
    static std::vector<IntVec2> coordsToRemove;
    coordsToRemove.clear();
    coordsToRemove.reserve(flowField.m_activeFlowFieldChunks.size());

    for (auto it : flowField.m_activeFlowFieldChunks)
    {
        FlowFieldChunk*& flowFieldChunk = it.second;
        if (!world.GetActiveChunk(flowFieldChunk->GetChunkCoords()))
        {
            coordsToRemove.push_back(flowFieldChunk->GetChunkCoords());
        }
        else
        {
            float distanceSquared = flowFieldChunk->GetChunkBounds().GetCenter().GetDistanceSquaredTo(anchorLocation);

            if (distanceSquared > flowFieldGenerationRadiusSquared)
            {
                coordsToRemove.push_back(flowFieldChunk->GetChunkCoords());
            }
        }
    }
    for (IntVec2 const& coords : coordsToRemove)
    {
        FlowFieldChunk*& flowChunk = flowField.m_activeFlowFieldChunks[coords];
        flowChunk->HardReset();
        delete flowChunk;
        flowField.m_activeFlowFieldChunks.erase(coords);
    }

    return static_cast<int>(coordsToRemove.size());
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::GenerateFlow(FlowField& flowField, WorldCoords const& destination)
{
    ScopedTimer timer("GenerateFlow");

    bool succeeded = flowField.Seed(destination);
    if (!succeeded)
    {
        g_devConsole->LogError("Failed to seed flow field");
    }

    flowField.m_openList.emplace(destination, 0.f);
    GenerateDistanceField(flowField);

    flowField.m_openList.emplace(destination, 0.f);
    GenerateGradient(flowField);
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::GenerateDistanceField(FlowField& flowField)
{
    ScopedTimer timer("- Generate Distance Field");
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();

    while (!flowField.m_openList.empty())
    {
        FlowGenerationCoords flowGenCoords = flowField.m_openList.top();
        flowField.m_openList.pop();

        IntVec2 const& currentChunkCoords = flowGenCoords.m_chunkCoords;
        IntVec2 const& currentLocalTileCoords = flowGenCoords.m_localTileCoords;

        FlowFieldChunk* currentChunk = flowField.GetActiveChunk(currentChunkCoords);
        if (!currentChunk)
        {
            continue;
        }

        int currentIndex = currentChunk->m_distanceField.GetIndexForCoords(currentLocalTileCoords);

        if (currentChunk->m_consideredCells.Get(currentIndex))
        {
            continue;
        }
        if (currentChunk->IsTileSolid(currentLocalTileCoords))
        {
            continue;
        }
        currentChunk->m_consideredCells.Set(currentIndex);

        for (IntVec2 const& neighborOffset : neighborOffsets)
        {
            WorldCoords neighborWorldCoords = world.GetWorldCoordsAtOffset(flowGenCoords, neighborOffset);
            FlowFieldChunk* neighborChunk = flowField.GetActiveChunk(neighborWorldCoords.m_chunkCoords);
            if (!neighborChunk)
            {
                continue;
            }
            int neighborLocalTileIndex = neighborChunk->m_distanceField.GetIndexForCoords(neighborWorldCoords.m_localTileCoords);
            if (neighborChunk->m_consideredCells.Get(neighborLocalTileIndex))
            {
                continue;
            }
            if (neighborChunk->IsTileSolid(neighborWorldCoords.m_localTileCoords))
            {
                continue;
            }

            int neighborCost = (int) neighborChunk->m_costField.Get(neighborWorldCoords.m_localTileCoords);

            float dx = FLT_MAX; // dx = min( neighborWestDistance, neighborEastDistance );
            float dy = FLT_MAX; // dy = min( neighborNorthDistance, neighborSouthDistance );

            // nofn = neighbor of neighbor
            for (IntVec2 const& nofnOffset : neighborOffsets)
            {
                WorldCoords nofnWorldCoords = world.GetWorldCoordsAtOffset(neighborWorldCoords, nofnOffset);
                FlowFieldChunk* nofnChunk = flowField.GetActiveChunk(nofnWorldCoords.m_chunkCoords);
                if (!nofnChunk)
                {
                    continue;
                }
                if (nofnChunk->IsTileSolid(nofnWorldCoords.m_localTileCoords))
                {
                    continue;
                }
                float nofnDistance = nofnChunk->m_distanceField.Get(nofnWorldCoords.m_localTileCoords);
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

            float delta = 2 * neighborCost - MathUtils::PowF(dx - dy, 2);
            float calculatedNeighborDistance;
            if (delta >= 0)
            {
                calculatedNeighborDistance = (dx + dy + MathUtils::SqrtF(delta)) / 2.f;
            }
            else
            {
                calculatedNeighborDistance = MathUtils::MinF(dx + neighborCost, dy + neighborCost);
            }

            if (calculatedNeighborDistance < neighborChunk->m_distanceField.Get(neighborWorldCoords.m_localTileCoords))
            {
                neighborChunk->m_distanceField.Set(neighborWorldCoords.m_localTileCoords, calculatedNeighborDistance);
                flowField.m_openList.push({ neighborWorldCoords.m_chunkCoords, neighborWorldCoords.m_localTileCoords, calculatedNeighborDistance });
            }
        }
    }
}

void SFlowField::GenerateGradient(FlowField& flowField)
{
    ScopedTimer timer("- Generate Gradient");
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();

    flowField.ResetConsideredCells();

    while (!flowField.m_openList.empty())
    {
        FlowGenerationCoords currentWorldCoords = flowField.m_openList.top();
        flowField.m_openList.pop();

        FlowFieldChunk* currentChunk = flowField.m_activeFlowFieldChunks.at(currentWorldCoords.m_chunkCoords);
        int currentIndex = currentChunk->m_gradient.GetIndexForCoords(currentWorldCoords.m_localTileCoords);
        if (currentChunk->m_consideredCells.Get(currentIndex))
        {
            continue;
        }
        if (currentChunk->IsTileSolid(currentWorldCoords.m_localTileCoords))
        {
            continue;
        }
        currentChunk->m_consideredCells.Set(currentIndex);

        float currentTileDistance = currentChunk->m_distanceField.Get(currentWorldCoords.m_localTileCoords);

        Vec2 gradient = Vec2::ZeroVector;
        for (IntVec2 const& neighborOffset : neighborOffsets)
        {
            WorldCoords neighborWorldCoords = world.GetWorldCoordsAtOffset(currentWorldCoords, neighborOffset);
            FlowFieldChunk* neighborChunk = flowField.GetActiveChunk(neighborWorldCoords.m_chunkCoords);
            if (!neighborChunk)
            {
                continue;
            }

            float currentNeighborDistance = (float) neighborChunk->m_distanceField.Get(neighborWorldCoords.m_localTileCoords);
            float dDist = currentTileDistance - currentNeighborDistance;

            if (neighborChunk->IsTileSolid(neighborWorldCoords.m_localTileCoords))
            {
                // Always treat solid walls as being 0.5 distance away in that direction, so that flow always generates away from walls without skewing too much
                // If we leave this as the actual distance, which is very large (999), then gradient will point away from walls too strongly
                dDist = -0.5;
            }

            if (neighborOffset.x != 0.f)
            {
                gradient.x += dDist * MathUtils::Sign(neighborOffset.x);
            }
            else
            {
                gradient.y += dDist * MathUtils::Sign(neighborOffset.y);
            }

            int neighborIndex = neighborChunk->m_gradient.GetIndexForCoords(neighborWorldCoords.m_localTileCoords);
            if (neighborChunk->m_consideredCells.Get(neighborIndex) == false)
            {
                flowField.m_openList.push({ neighborWorldCoords.m_chunkCoords, neighborWorldCoords.m_localTileCoords, currentNeighborDistance });
            }
        }

        gradient.Normalize();

        currentChunk->m_gradient.Set(currentWorldCoords.m_localTileCoords, gradient);
    }
}
