// Bradley Christensen - 2022-2025
#include "SCollisionHash.h"
#include "CCollision.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "SCCollision.h"
#include "Chunk.h"



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::Startup()
{
	AddWriteDependencies<SCWorld, SCCollision>();
    AddReadDependencies<CCollision, CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::Run(SystemContext const& context)
{
    auto& world = g_ecs->GetSingleton<SCWorld>();
    auto& scCollision = g_ecs->GetSingleton<SCCollision>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collStorage = g_ecs->GetArrayStorage<CCollision>();

    for (auto& chunk : world.m_activeChunks)
    {
        IntVec2 const& chunkCoords = chunk.first;
        if (scCollision.m_chunkCollisionData.find(chunkCoords) == scCollision.m_chunkCollisionData.end())
        {
            // initialize chunk collision data
            ChunkCollisionData& chunkCollisionData = scCollision.m_chunkCollisionData[chunkCoords];
            chunkCollisionData.m_entitiesTouchingChunk.reserve(64);
            chunkCollisionData.m_entitiesTouchingTile.resize(world.GetNumTilesInChunk());
		}
	}

    // clear out state data and remove any chunk entries that are no longer loaded
    for (auto it = scCollision.m_chunkCollisionData.begin(); it != scCollision.m_chunkCollisionData.end();)
    {
        // Clear out stale data
        it->second.m_entitiesTouchingChunk.clear();
        for (auto& entitiesTouchingTile : it->second.m_entitiesTouchingTile)
        {
            entitiesTouchingTile.clear();
        }

        if (!world.IsChunkLoaded(it->first))
        {
            it = scCollision.m_chunkCollisionData.erase(it);
        }
        else
        {
            it++;
        }
    }

    // Hash all entities
    for (GroupIter it = g_ecs->Iterate<CTransform, CCollision>(context); it.IsValid(); ++it)
    {
        CTransform& trans = transStorage[it];
        CCollision& coll = *collStorage.Get(it);

        Vec2& posA = trans.m_pos;
        float& radiusA = coll.m_radius;

        world.ForEachChunkCoordsInCircle(posA, radiusA, [&](IntVec2 const& chunkCoords)
        {
            scCollision.m_chunkCollisionData[chunkCoords].m_entitiesTouchingChunk.push_back(it.m_currentIndex);
            return true; // continue iterating
        });

        world.ForEachWorldCoordsOverlappingCapsule(posA, posA, radiusA, [&](WorldCoords& worldCoords)
        {
            auto& chunkCollisionData = scCollision.m_chunkCollisionData[worldCoords.m_chunkCoords];
            int tileIndex = world.GetActiveChunk(worldCoords.m_chunkCoords)->m_tileIDs.GetIndexForCoords(worldCoords.m_localTileCoords);
            chunkCollisionData.m_entitiesTouchingTile[tileIndex].push_back(it.m_currentIndex);
            return true; // continue iterating
        });
    }
}