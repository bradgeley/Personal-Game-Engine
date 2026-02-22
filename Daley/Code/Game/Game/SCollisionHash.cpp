// Bradley Christensen - 2022-2026
#include "SCollisionHash.h"
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
    scCollision.m_tileBuckets.resize(StaticWorldSettings::s_numTilesInPlayableWorld);
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::PreRun()
{
    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();

    for (auto& bucket : scCollision.m_tileBuckets)
    {
        bucket.clear();
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

        if (coll.GetIsSingleHash())
        {
			IntVec2 worldCoords = world.GetTileCoordsAtWorldPosClamped(pos);
            int index = world.m_tiles.GetIndexForCoords(worldCoords);
            scCollision.m_tileBuckets[index].push_back(it.GetEntityID());
        }
        else
        {
            float radius = coll.m_radius + StaticWorldSettings::s_collisionHashWiggleRoom;
            world.ForEachPlayableTileOverlappingCircle(pos, radius, [&](IntVec2 const& worldCoords)
            {
                int index = world.m_tiles.GetIndexForCoords(worldCoords);
                if (world.m_tiles.IsValidIndex(index))
                {
                    scCollision.m_tileBuckets[index].push_back(it.GetEntityID());
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
