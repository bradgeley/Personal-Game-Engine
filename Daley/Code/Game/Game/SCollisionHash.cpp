// Bradley Christensen - 2022-2026
#include "SCollisionHash.h"
#include "CollisionProfile.h"
#include "CCollision.h"
#include "CMovement.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "SCCollision.h"
#include "WorldSettings.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::Startup()
{
    AddReadDependencies<CMovement, CTransform>();
	AddWriteDependencies<CCollision, SCCollision, SCWorld>();

    m_runWhilePaused = false;

    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();

    // Each collision channel gets a layer for hashing
	scCollision.m_collisionLayers.resize((size_t) CollisionChannel::Count);

    for (CollisionLayer& layer : scCollision.m_collisionLayers)
    {
        layer.resize(StaticWorldSettings::s_numTilesInPlayableWorld);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void HashEntity(CollisionLayer& layer, SCCollision& scCollision, SCWorld& world, CCollision& coll, Vec2 const& pos, EntityID entityID)
{
	coll.SetHasBeenHashed(true);

    if (coll.GetIsSingleHash())
    {
        IntVec2 worldCoords = world.GetTileCoordsAtWorldPosClamped(pos);
        int index = world.m_tiles.GetIndexForCoords(worldCoords);
        layer[index].push_back(entityID);
        scCollision.m_dirtyBuckets.insert(index);
    }
    else
    {
        float radius = coll.m_radius + StaticWorldSettings::s_collisionHashWiggleRoom;
        world.ForEachPlayableTileOverlappingCircle(pos, radius, [&](IntVec2 const& worldCoords)
        {
            int index = world.m_tiles.GetIndexForCoords(worldCoords);
            if (world.m_tiles.IsValidIndex(index))
            {
                layer[index].push_back(entityID);
                scCollision.m_dirtyBuckets.insert(index);
            }
            return true;
        });
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::Run(SystemContext const& context) const
{
    // Read dependencies
    auto const& moveStorage = context.GetArrayStorageConst<CMovement>();
    auto const& transStorage = context.GetArrayStorageConst<CTransform>();

    // Write Dependencies
    auto& collStorage = context.GetArrayStorage<CCollision>();
    SCCollision& scCollision = context.GetSingleton<SCCollision>();
    SCWorld& world = context.GetSingleton<SCWorld>();

    BitMask movementBitMask = context.GetComponentBitMask<CMovement>();

    world.m_numEnemiesInTile.SetAll(0);

	auto dirtyBucketsCopy = scCollision.m_dirtyBuckets;

    // Remove all entities that are invalid or moved, leaving the static ones
    for (int const& bucketIndex : dirtyBucketsCopy)
    {
		int numEntitiesInBucket = 0;
        for (CollisionLayer& layer : scCollision.m_collisionLayers)
        {
            CollisionBucket& bucket = layer[bucketIndex];
			numEntitiesInBucket += static_cast<int>(bucket.size());

            for (int entityIndex = static_cast<int>(bucket.size() - 1); entityIndex >= 0; --entityIndex)
            {
                EntityID entityID = bucket[entityIndex];
                if (context.IsValid(entityID))
                {
                    bool hasMovementComponent = context.HasComponentsUnsafe(entityID.GetIndex(), movementBitMask);
                    CMovement const* move = hasMovementComponent ? moveStorage.Get(entityID) : nullptr;
                    if (!move || !move->GetMovedThisFrame())
                    {
                        // Entity couldn't have moved
                        continue;
                    }
                }
                
                // Todo: Remove entities whose collision component was disabled or removed

                // Remove entity from hash table
                bucket[entityIndex] = bucket.back();
                bucket.pop_back();
                numEntitiesInBucket--;
				continue;
            }
        }
		if (numEntitiesInBucket == 0)
		{
			scCollision.m_dirtyBuckets.erase(bucketIndex);
		}
    }


    // Iterate over all collision components and hash them
    for (GroupIter it = context.Iterate<CTransform, CCollision>(); it.IsValid(); ++it)
    {
        CCollision& coll = collStorage[it];
        if (!coll.IsCollisionEnabled())
        {
            continue;
        }

        if (coll.GetHasBeenHashed())
        {
            bool hasMovementComponent = context.HasComponentsUnsafe(it.m_currentIndex, movementBitMask);
            CMovement const* move = hasMovementComponent ? moveStorage.Get(it) : nullptr;
            if (!move || !move->GetMovedThisFrame())
            {
                // Entity couldn't have moved, and already was hashed
                // Assumption: All enemies have a movement component, so we aren't skipping adding enemy count to tiles below
                continue;
            }
        }

        CTransform const& trans = transStorage[it];
        Vec2 pos = trans.m_pos + coll.m_offset;
        CollisionChannel channel = coll.m_collisionProfile.m_objectChannel;
        if (channel == CollisionChannel::Enemy)
        {
            // Count enemies in tiles, for things like blocking tower placement in those tiles
            IntVec2 worldCoords = world.GetTileCoordsAtWorldPosClamped(pos);
            world.m_numEnemiesInTile.GetRef(worldCoords)++;
        }

		CollisionLayer& layer = scCollision.GetCollisionLayer(channel);
        HashEntity(layer, scCollision, world, coll, pos, it.GetEntityID());
    }
}
