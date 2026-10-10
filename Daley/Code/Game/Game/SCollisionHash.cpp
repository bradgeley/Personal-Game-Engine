// Bradley Christensen - 2022-2026
#include "SCollisionHash.h"
#include "CollisionChannel.h"
#include "CCollision.h"
#include "CMovement.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "SCCollision.h"
#include "WorldSettings.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/ECS/SystemContext.h"
#include <algorithm>
#include <unordered_set>
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void ValidateEnemyBuckets(SystemContext const& context);



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::Startup()
{
    AddReadDependencies<CMovement, CTransform, SCWorld>();
	AddWriteDependencies<CCollision, SCCollision>();

    m_runWhilePaused = false;
    m_systemSplittingNumJobs = std::thread::hardware_concurrency() - 1;
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::Run(SystemContext const& context) const
{
    // Read dependencies
    auto const& moveStorage = context.GetArrayStorageConst<CMovement>();
    auto const& transStorage = context.GetArrayStorageConst<CTransform>();
    SCWorld const& world = context.GetSingletonConst<SCWorld>();

    // Write Dependencies
    auto& collStorage = context.GetArrayStorage<CCollision>();

    BitMask movementBitMask = context.GetComponentBitMask<CMovement>();

    for (auto it = context.Iterate<CCollision, CTransform>(); it.IsValid(); ++it)
    {
        CCollision& collision = collStorage[it];
        if (!collision.IsCollisionEnabled())
        {
            continue;
        }

		bool hasBeenHashed = collision.m_spatialPartitionTileIndex != -1;
        if (hasBeenHashed && context.HasComponentsUnsafe(it.m_currentIndex, movementBitMask))
        {
            CMovement const& move = moveStorage[it];
            if (!move.GetMovedThisFrame())
            {
                // Early out for stationary things that have already been hashed
                continue;
            }
        }

        CTransform const& transform = transStorage[it];
        collision.m_spatialPartitionTileIndex = world.GetTileIndexAtWorldPos(transform.m_pos);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::PostRun(SystemContext const& context) const
{
    SCCollision& scCollision = context.GetSingleton<SCCollision>();

    // Read Dependencies
    auto const& collStorage = context.GetArrayStorageConst<CCollision>();

    // Create sorted entity list
    scCollision.m_sortedEnemies.reserve(MAX_ENTITIES);
    scCollision.m_activeEnemyBuckets.reserve(StaticWorldSettings::s_numTilesInWorld);
    scCollision.m_enemySortBuckets.resize(StaticWorldSettings::s_numTilesInWorld);
    scCollision.m_enemyBucketIndices.resize(StaticWorldSettings::s_numTilesInWorld);

    scCollision.m_sortedEnemies.clear();

    auto& sortBuckets = scCollision.m_enemySortBuckets;
    auto& activeBuckets = scCollision.m_activeEnemyBuckets;

    // Clear previous frame buckets
    for (int tileIndex : activeBuckets)
    {
        sortBuckets[tileIndex].clear();
    }
    scCollision.m_activeEnemyBuckets.clear();

    // Build buckets
    for (auto it = context.Iterate<CCollision>(); it.IsValid(); ++it)
    {
        CCollision const& collision = collStorage[it];

        if (!collision.IsCollisionEnabled())
        {
            continue;
        }

        if (collision.m_collisionChannel != CollisionChannel::Enemy)
        {
            continue;
        }

        int tileIndex = collision.m_spatialPartitionTileIndex;

        if (tileIndex == -1)
        {
            continue;
        }

        auto& bucket = sortBuckets[tileIndex];
        if (bucket.empty())
        {
            activeBuckets.push_back(tileIndex);
        }

        bucket.push_back(it.GetEntityID());
    }

    std::sort(activeBuckets.begin(), activeBuckets.end());

    for (int tileIndex : activeBuckets)
    {
        auto& bucket = sortBuckets[tileIndex];

        if (bucket.empty())
        {
            continue;
        }

        scCollision.m_sortedEnemies.insert(scCollision.m_sortedEnemies.end(), bucket.begin(), bucket.end());
    }

	// Fill tiles with no enemies with the next tile's index, so that our query algorithm works
    // The query algorithm relies on the fact that m_enemyBucketIndices[tileIndex] and m_enemyBucketIndices[tileIndex+1] 
    // will have a range of 0 if there are no enemies in the tile, so they need to match.
    int sortedIndex = 0;
    int const numEnemies = static_cast<int>(scCollision.m_sortedEnemies.size());

    for (int tileIndex = 0; tileIndex < StaticWorldSettings::s_numTilesInWorld; ++tileIndex)
    {
        while (sortedIndex < numEnemies)
        {
            CCollision const& collision = collStorage[scCollision.m_sortedEnemies[sortedIndex]];

            if (collision.m_spatialPartitionTileIndex >= tileIndex)
            {
                break;
            }

            ++sortedIndex;
        }

        scCollision.m_enemyBucketIndices[tileIndex] = sortedIndex;
    }
}



//----------------------------------------------------------------------------------------------------------------------
void ValidateEnemyBuckets(SystemContext const& context)
{
    auto const& scCollision = context.GetSingletonConst<SCCollision>();
    auto const& collStorage = context.GetArrayStorageConst<CCollision>();
    int numTiles = StaticWorldSettings::s_numTilesInWorld;

    auto const& sortedEnemies = scCollision.m_sortedEnemies;
    auto const& bucketIndices = scCollision.m_enemyBucketIndices;

    // Track duplicates
    std::unordered_set<EntityID> seen;
    seen.reserve(sortedEnemies.size());

    int lastTile = -1;

    for (int i = 0; i < sortedEnemies.size(); ++i)
    {
        EntityID id = sortedEnemies[i];
        CCollision const& collision = collStorage[id];

        // Validate entity properties
        ASSERT_OR_DIE(collision.IsCollisionEnabled(), "Collision is not enabled for entity");
        ASSERT_OR_DIE(collision.m_collisionChannel == CollisionChannel::Enemy, "Collision channel is not Enemy");

        int tile = collision.m_spatialPartitionTileIndex;

        ASSERT_OR_DIE(tile >= 0, "Tile index is negative");
        ASSERT_OR_DIE(tile < numTiles, "Tile index is out of range");

        // Validate sorting
        ASSERT_OR_DIE(tile >= lastTile, "Tiles are not sorted");
        lastTile = tile;

        // Validate duplicates
        auto [it, inserted] = seen.insert(id);
        ASSERT_OR_DIE(inserted, "Duplicate entity found");
    }


    // Validate bucket indices
    for (int tile = 0; tile < numTiles; ++tile)
    {
        int index = bucketIndices[tile];

        ASSERT_OR_DIE(index >= 0, "Bucket index is negative");
        ASSERT_OR_DIE(index <= sortedEnemies.size(), "Bucket index is out of range");

        if (index < sortedEnemies.size())
        {
            CCollision const& collision = collStorage[sortedEnemies[index]];

            // First entity at this index must belong to this tile
            // unless this is an empty bucket pointing at the next tile
            if (collision.m_spatialPartitionTileIndex == tile)
            {
                ASSERT_OR_DIE(true, "First entity at this index does not belong to this tile");
            }
        }
    }


    // Validate that bucket indices point to the first entity of each tile
    for (int i = 0; i < sortedEnemies.size();)
    {
        EntityID id = sortedEnemies[i];
        int tile = collStorage[id].m_spatialPartitionTileIndex;

        ASSERT_OR_DIE(bucketIndices[tile] == i, "Bucket index does not point to the first entity of the tile");

        int start = i;

        while (i < sortedEnemies.size() &&
               collStorage[sortedEnemies[i]].m_spatialPartitionTileIndex == tile)
        {
            ++i;
        }

        // Everything in this range belongs to the same tile
        for (int j = start; j < i; ++j)
        {
            ASSERT_OR_DIE(collStorage[sortedEnemies[j]].m_spatialPartitionTileIndex == tile, "Entity does not belong to the expected tile");
        }
    }
}