// Bradley Christensen - 2022-2025
#include "SWorldCollision.h"
#include "CCollision.h"
#include "CTransform.h"
#include "SCCollision.h"
#include "SCWorld.h"
#include "Engine/Math/GeometryUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorldCollision::Startup()
{
    AddWriteDependencies<CTransform>();
    AddReadDependencies<CCollision, SCWorld, SCCollision>();
}



//----------------------------------------------------------------------------------------------------------------------
void SWorldCollision::Run(SystemContext const&)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collStorage = g_ecs->GetArrayStorage<CCollision>();
    SCWorld const& scWorld = g_ecs->GetSingleton<SCWorld>();
    SCCollision const& scCollision = g_ecs->GetSingleton<SCCollision>();

    for (int tileIndex = 0; tileIndex < scWorld.m_tiles.Count(); ++tileIndex)
    {
        if (!scWorld.m_tiles.Get(tileIndex).IsSolid())
        {
            continue;
        }

		EntityBucket const& tileBucket = scCollision.m_tileBuckets[tileIndex];

        IntVec2 tileCoords = scWorld.m_tiles.GetCoordsForIndex(tileIndex);
		AABB2 tileBounds = scWorld.GetTileBounds(tileCoords);
        
        for (EntityID const& entity : tileBucket)
        {
			CCollision const& collision = collStorage[entity];
            CTransform& transform = transStorage[entity];

			Vec2 newPos = transform.m_pos + collision.m_offset;
			bool collisionResolved = GeometryUtils::PushDiscOutOfAABB2D(newPos, collision.m_radius + 0.0001f, tileBounds);
            if (collisionResolved)
            {
                transform.m_pos = newPos - collision.m_offset;
            }
        }

        continue;
    }
}