// Bradley Christensen - 2022-2025
#include "SCollisionHash.h"
#include "SCCamera.h"
#include "CCollision.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "SCCollision.h"
#include "Engine/Core/ErrorUtils.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::Startup()
{
	AddWriteDependencies<SCWorld, SCCollision>();
    AddReadDependencies<CCollision, CTransform, SCCamera>();

    // Hashing entities can be split, since it is a read only operation until the combine phase
    int numThreads = std::thread::hardware_concurrency() - 1;
	m_systemSplittingNumJobs = numThreads - 1;
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::PreRun()
{

}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::Run(SystemContext const& context)
{
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();
    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collStorage = g_ecs->GetArrayStorage<CCollision>();

    // Hash entities for this split system
    for (GroupIter it = g_ecs->Iterate<CTransform, CCollision>(context); it.IsValid(); ++it)
    {
        CCollision& coll = *collStorage.Get(it);
        if (!coll.IsCollisionEnabled())
        {
            continue;
        }

        CTransform& trans = transStorage[it];

        Vec2 pos = trans.m_pos + coll.m_offset;
        float radius = coll.m_radius + world.m_worldSettings.m_collisionHashWiggleRoom;

        world.ForEachWorldCoordsOverlappingCircle(pos, radius, [&](IntVec2 const& worldCoords)
        {								  
            int tileIndex = world.m_tiles.GetIndexForCoords(worldCoords);
            if (world.m_tiles.IsValidIndex(tileIndex))
            {
                scCollision.m_tileBuckets[tileIndex].push_back(it.m_currentIndex);
            }
            return true;
        });
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionHash::PostRun()
{

}
