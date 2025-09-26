// Bradley Christensen - 2022-2025
#include "SCollisionHash.h"
#include "CCamera.h"
#include "CCollision.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "SCCollision.h"
#include "Chunk.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Performance/ScopedTimer.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::Startup()
{
	AddWriteDependencies<SCWorld, SCCollision>();
    AddReadDependencies<CCollision, CTransform, CCamera>();

    // Hashing entities can be split, since it is a read only operation until the combine phase
    int numThreads = std::thread::hardware_concurrency() - 1;
	m_systemSplittingNumJobs = numThreads - 1;
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::PreRun()
{
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();
    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();

    // clear out stale data and remove any chunk entries that are no longer loaded
    for (auto it = scCollision.m_chunkCollisionData.begin(); it != scCollision.m_chunkCollisionData.end();)
    {
		IntVec2 const& chunkCoords = it->first;
		ChunkCollisionData& chunkCollisionData = it->second;

        // Clear out stale data
        chunkCollisionData.m_chunkBucket.clear();
        for (auto& tileBucket : chunkCollisionData.m_tileBuckets)
        {
            tileBucket.second.clear();
        }

        if (!world.IsChunkLoaded(chunkCoords))
        {
            // Erase the main collision data for this chunk
            it = scCollision.m_chunkCollisionData.erase(it);

			// Erase the per-thread collision data for this chunk
            for (auto& perThreadCollisionData : scCollision.m_perThreadCollisionData)
            {
                perThreadCollisionData.erase(chunkCoords);
			}
        }
        else
        {
            it++;
        }
    }

	// Optimization: only update collision hashes for entities that are within the collision hash radius of the active camera
    scCollision.m_collisionUpdateBounds = AABB2::ZeroToOne;

    SystemContext context;
    for (auto it = g_ecs->Iterate<CTransform, CCamera>(context); it.IsValid(); ++it)
    {
        CCamera& camera = *g_ecs->GetComponent<CCamera>(it);
        if (camera.m_isActive)
        {
            scCollision.m_collisionUpdateBounds = camera.m_camera.GetTranslatedOrthoBounds2D();
            scCollision.m_collisionUpdateBounds.SetDimsAboutCenter(Vec2(StaticWorldSettings::s_collisionHashRadius * 2.f, StaticWorldSettings::s_collisionHashRadius * 2.f));
            break;
        }
    }

	scCollision.m_perThreadCollisionData.resize(m_systemSplittingNumJobs);
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::Run(SystemContext const& context)
{
    auto& world = g_ecs->GetSingleton<SCWorld>();
    auto& scCollision = g_ecs->GetSingleton<SCCollision>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collStorage = g_ecs->GetArrayStorage<CCollision>();

    // Hash entities for this split system
    for (GroupIter it = g_ecs->Iterate<CTransform, CCollision>(context); it.IsValid(); ++it)
    {
        CTransform& trans = transStorage[it];
        CCollision& coll = *collStorage.Get(it);

        Vec2 pos = trans.m_pos + coll.m_offset;
        float radius = coll.m_radius + world.m_worldSettings.m_collisionHashWiggleRoom;

        if (!scCollision.m_collisionUpdateBounds.IsPointInside(pos))
        {
            continue;
		}

		std::unordered_map<IntVec2, ChunkCollisionData>& thisThreadCollisionData = context.m_didSystemSplit ? scCollision.m_perThreadCollisionData[context.m_systemSplittingJobID] : scCollision.m_chunkCollisionData;

        world.ForEachActiveChunkOverlappingCircle(pos, radius, [&](Chunk const& chunk)
        {
            thisThreadCollisionData[chunk.m_chunkCoords].m_chunkBucket.push_back(it.m_currentIndex);
            return true; // continue iterating
        });

        world.ForEachWorldCoordsOverlappingCircle(pos, radius, [&](WorldCoords const& worldCoords)
        {
            ChunkCollisionData& threadChunkCollisionData = thisThreadCollisionData[worldCoords.m_chunkCoords];
            int tileIndex = world.GetActiveChunk(worldCoords.m_chunkCoords)->m_tiles.GetIndexForCoords(worldCoords.m_localTileCoords);
            threadChunkCollisionData.m_tileBuckets[tileIndex].push_back(it.m_currentIndex);
            return true; // continue iterating
        });
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::PostRun()
{
    auto& scCollision = g_ecs->GetSingleton<SCCollision>();

    int numEntitiesHashed = 0;
    int numEntitiesHashedTiles = 0;

    for (auto& perThreadCollisionData : scCollision.m_perThreadCollisionData)
    {
        for (auto& chunkCollisionPair : perThreadCollisionData)
        {
            IntVec2 const& chunkCoords = chunkCollisionPair.first;
            ChunkCollisionData& threadChunkCollisionData = chunkCollisionPair.second;
            ChunkCollisionData& mainChunkCollisionData = scCollision.m_chunkCollisionData[chunkCoords];

            mainChunkCollisionData.m_chunkBucket.insert(mainChunkCollisionData.m_chunkBucket.end(), threadChunkCollisionData.m_chunkBucket.begin(), threadChunkCollisionData.m_chunkBucket.end());
			numEntitiesHashed += static_cast<int>(threadChunkCollisionData.m_chunkBucket.size());

            threadChunkCollisionData.m_chunkBucket.clear();

            for (auto it = threadChunkCollisionData.m_tileBuckets.begin(); it != threadChunkCollisionData.m_tileBuckets.end(); ++it)
            {
                EntityBucket& threadTileBucket = it->second;
                EntityBucket& mainTileBucket = mainChunkCollisionData.m_tileBuckets[it->first];
                mainTileBucket.insert(mainTileBucket.end(), threadTileBucket.begin(), threadTileBucket.end());
				numEntitiesHashedTiles += static_cast<int>(threadTileBucket.size());

                threadTileBucket.clear();
            }
        }
	}

	ASSERT_OR_DIE(numEntitiesHashedTiles >= numEntitiesHashed, "SCollisionHash::PostRun - Mismatch in number of entities hashed between chunk and tile buckets.");
}
