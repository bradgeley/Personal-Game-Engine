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
void SCollision::Run(SystemContext const&)
{
    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();

	auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& collisionStorage = g_ecs->GetArrayStorage<CCollision>();

	SCWorld const& scWorld = g_ecs->GetSingleton<SCWorld>();
    scWorld.ForEachChunkCoordsOverlappingAABB(scCollision.m_collisionUpdateBounds, [&](IntVec2 const& chunkCoords)
    {
        ChunkCollisionData const& chunkCollisionData = scCollision.m_chunkCollisionData[chunkCoords];
        auto& tileBuckets = chunkCollisionData.m_tileBuckets;

        for (auto tileIt = tileBuckets.begin(); tileIt != tileBuckets.end(); ++tileIt)
        {
            EntityBucket const& tileBucket = tileIt->second;

            // Handle collision inside tile
            for (size_t a = 0; a < tileBucket.size(); ++a)
            {
                EntityID const& entityA = tileBucket[a];
                CTransform& transformA = transformStorage[entityA];
                CCollision const& collisionA = collisionStorage[entityA];

                bool canBpushA = !collisionA.IsImmovable();

                for (size_t b = a + 1; b < tileBucket.size(); ++b)
                {
                    EntityID const& entityB = tileBucket[b];
                    CTransform& transformB = transformStorage[entityB];
                    CCollision const& collisionB = collisionStorage[entityB];

					Vec2 newPosA = transformA.m_pos + collisionA.m_offset;
					Vec2 newPosB = transformB.m_pos + collisionB.m_offset;

                    bool canApushB = !collisionB.IsImmovable();

                    bool collisionResolved = false;
                    if (canApushB && canBpushA)
                    {
                        collisionResolved = GeometryUtils::PushDiscsOutOfEachOther2D(newPosA, collisionA.m_radius, newPosB, collisionB.m_radius, true, 0.75);
                    }
                    else if (canApushB) // && !canBpushA
                    {
                        collisionResolved = GeometryUtils::PushDiscOutOfDisc2D(newPosB, collisionB.m_radius, newPosA, collisionA.m_radius);
                    }
                    else if (canBpushA) // && !canApushB
                    {
                        collisionResolved = GeometryUtils::PushDiscOutOfDisc2D(newPosA, collisionA.m_radius, newPosB, collisionB.m_radius);
                    }

                    if (collisionResolved)
                    {
                        // Make sure we didnt push into a wall
                        if (!scWorld.IsPointInsideSolidTile(newPosA))
                        {
                            transformA.m_pos = newPosA - collisionA.m_offset;
                        }
                        if (!scWorld.IsPointInsideSolidTile(newPosB))
                        {
                            transformB.m_pos = newPosB - collisionB.m_offset;
                        }
                    }
                }
            }
        }

        return true; // keep iterating
	});
}