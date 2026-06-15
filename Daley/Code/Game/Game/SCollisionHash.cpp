// Bradley Christensen - 2022-2026
#include "SCollisionHash.h"
#include "CollisionProfile.h"
#include "CCollision.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "SCCollision.h"
#include "WorldSettings.h"
#include "Engine/Core/ErrorUtils.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::Startup()
{
    AddReadDependencies<SCWorld, CCollision, CTransform>();
	AddWriteDependencies<SCCollision>();

    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();

    // Each collision channel gets a layer for hashing
	scCollision.m_collisionLayers.resize((size_t) CollisionChannel::Count);

    for (CollisionLayer& layer : scCollision.m_collisionLayers)
    {
        layer.resize(StaticWorldSettings::s_numTilesInPlayableWorld);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::Run(SystemContext const& context) const
{
    // Read dependencies
    SCWorld const& world = context.GetSingletonConst<SCWorld>();
    auto& transStorage = context.GetArrayStorageConst<CTransform>();
    auto& collStorage = context.GetArrayStorageConst<CCollision>();

    // Write Dependencies
    SCCollision& scCollision = context.GetSingleton<SCCollision>();

    // Clear out old data
    for (int dirtyBucket : scCollision.m_dirtyBuckets)
    {
        for (CollisionLayer& layer : scCollision.m_collisionLayers)
        {
            CollisionBucket& bucket = layer[dirtyBucket];
            bucket.clear();
        }
    }
    scCollision.m_dirtyBuckets.clear();

    for (GroupIter it = context.Iterate<CTransform, CCollision>(); it.IsValid(); ++it)
    {
        CCollision const& coll = collStorage[it];
        if (!coll.IsCollisionEnabled())
        {
            continue;
        }
    
        CTransform const& trans = transStorage[it];
    
        Vec2 pos = trans.m_pos + coll.m_offset;

		CollisionChannel channel = coll.m_collisionProfile.m_objectChannel;
		CollisionLayer& layer = scCollision.GetCollisionLayer(channel);

        if (coll.GetIsSingleHash())
        {
			IntVec2 worldCoords = world.GetTileCoordsAtWorldPosClamped(pos);
            int index = world.m_tiles.GetIndexForCoords(worldCoords);
            layer[index].push_back(it.GetEntityID());
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
                    layer[index].push_back(it.GetEntityID());
                    scCollision.m_dirtyBuckets.insert(index);
                }
                return true;
            });
        }
    }
}
