// Bradley Christensen - 2022-2025
#include "SCollision.h"
#include "CCollision.h"
#include "SCCollision.h"
#include "SCWorld.h"
#include "CTransform.h"
#include "Engine/Math/GeometryUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Startup()
{
    AddWriteDependencies<CTransform>();
    AddReadDependencies<SCCollision, SCWorld, CCollision>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Run(SystemContext const& context)
{
    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();

	auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& collisionStorage = g_ecs->GetArrayStorage<CCollision>();

	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
    scWorld.ForEachChunkCoordsOverlappingAABB(scCollision.m_collisionUpdateBounds, [&](IntVec2 const& chunkCoords)
    {
        ChunkCollisionData& chunkCollisionData = scCollision.m_chunkCollisionData[chunkCoords];
        auto& tileBuckets = chunkCollisionData.m_tileBuckets;

        for (auto tileIt = tileBuckets.begin(); tileIt != tileBuckets.end(); ++tileIt)
        {
            EntityBucket& tileBucket = tileIt->second;

            // Handle collision inside tile
            for (size_t a = 0; a < tileBucket.size(); ++a)
            {
                EntityID& entityA = tileBucket[a];
                CTransform& transformA = transformStorage[entityA];
                CCollision& collisionA = collisionStorage[entityA];

                for (size_t b = a + 1; b < tileBucket.size(); ++b)
                {
                    EntityID& entityB = tileBucket[b];
                    CTransform& transformB = transformStorage[entityB];
                    CCollision& collisionB = collisionStorage[entityB];

                    if (GeometryUtils::PushDiscsOutOfEachOther2D(transformA.m_pos, collisionA.m_radius, transformB.m_pos, collisionB.m_radius, false))
                    {
                        // Handle collision between entityA and entityB
                    }
                }
            }
        }

        return true; // keep iterating
	});
}