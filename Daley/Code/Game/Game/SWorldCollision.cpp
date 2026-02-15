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
    AddReadDependencies<CCollision, SCWorld>();
}



//----------------------------------------------------------------------------------------------------------------------
void SWorldCollision::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collStorage = g_ecs->GetArrayStorage<CCollision>();
    SCWorld const& world = g_ecs->GetSingleton<SCWorld>();

    //for (auto it = g_ecs->Iterate<CTransform, CCollision>(context); it.IsValid(); ++it)
    //{
    //    CTransform& transform = transStorage[it];
    //    CCollision const& collision = collStorage[it];
    //
    //    if (collision.GetMustStayOnPath())
    //    {
    //        world.ForEachPlayableTileOverlappingCircle(transform.m_pos + collision.m_offset, collision.m_radius, [&](IntVec2 const& tileCoords)
    //        {
    //            AABB2 tileBounds = world.GetTileBounds(tileCoords);
    //            if (!world.IsTileOnPath(tileCoords))
    //            {
    //                Vec2 newPos = transform.m_pos + collision.m_offset;
    //                bool collisionResolved = GeometryUtils::PushDiscOutOfAABB2D(newPos, collision.m_radius + 0.0001f, tileBounds);
    //                if (collisionResolved)
    //                {
    //                    transform.m_pos = newPos - collision.m_offset;
    //                }
    //            }
    //            return true; // keep iterating
    //        });
	//	}
    //};

}