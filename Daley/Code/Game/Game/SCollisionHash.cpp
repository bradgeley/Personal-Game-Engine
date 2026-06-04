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
	AddWriteDependencies<SCWorld, SCCollision>();
    AddReadDependencies<CCollision, CTransform>();

    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();

    // Each collision channel gets a layer for hashing
	scCollision.m_collisionLayers.resize((size_t) CollisionChannel::Count);

    for (CollisionLayer& layer : scCollision.m_collisionLayers)
    {
        layer.resize(StaticWorldSettings::s_numTilesInPlayableWorld);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::PreRun()
{
    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();

    for (CollisionLayer& layer : scCollision.m_collisionLayers)
    {
        for (CollisionBucket& bucket : layer)
        {
            bucket.clear();
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::Run(SystemContext const& context)
{
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();
    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collStorage = g_ecs->GetArrayStorage<CCollision>();

    for (GroupIter it = g_ecs->Iterate<CTransform, CCollision>(context); it.IsValid(); ++it)
    {
        CCollision const& coll = *collStorage.Get(it);
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
                }
                return true;
            });
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::PostRun()
{

}
