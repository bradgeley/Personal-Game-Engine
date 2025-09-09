// Bradley Christensen - 2022-2025
#include "SCollision.h"
#include "CCollision.h"
#include "SCCollision.h"
#include "CTransform.h"
#include "Engine/Math/GeometryUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Startup()
{
    AddWriteDependencies<CTransform>();
    AddReadDependencies<SCCollision, CCollision>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Run(SystemContext const&)
{
    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();

	auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& collisionStorage = g_ecs->GetArrayStorage<CCollision>();

    for (auto it = scCollision.m_chunkCollisionData.begin(); it != scCollision.m_chunkCollisionData.end(); ++it)
    {
        std::vector<std::vector<EntityID>>& entitiesTouchingTiles = it->second.m_entitiesTouchingTile;

        for (auto tileIt = entitiesTouchingTiles.begin(); tileIt != entitiesTouchingTiles.end(); ++tileIt)
        {
            std::vector<EntityID>& entitiesInTile = *tileIt;

            // Handle collision inside tile
            for (size_t a = 0; a < entitiesInTile.size(); ++a)
            {
                EntityID& entityA = entitiesInTile[a];
                CTransform& transformA = transformStorage[entityA];
                CCollision& collisionA = collisionStorage[entityA];

                for (size_t b = a + 1; b < entitiesInTile.size(); ++b)
                {
                    EntityID& entityB = entitiesInTile[b];
                    CTransform& transformB = transformStorage[entityB];
                    CCollision& collisionB = collisionStorage[entityB];

                    if (GeometryUtils::PushDiscsOutOfEachOther2D(transformA.m_pos, collisionA.m_radius, transformB.m_pos, collisionB.m_radius))
                    {
                        // Handle collision between entityA and entityB
                    }
                }
            }
		}

	}
}