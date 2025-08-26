// Bradley Christensen - 2022-2025
#include "SWorldCollision.h"
#include "CCollision.h"
#include "CTransform.h"
#include "CRender.h"
#include "Chunk.h"
#include "SCDebug.h"
#include "SCWorld.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Math/GeometryUtils.h"
#include "WorldRaycast.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorldCollision::Startup()
{
    AddWriteDependencies<CTransform>();
    AddReadDependencies<CCollision, SCWorld>();
    AddWriteDependencies<SCDebug>();
}



//----------------------------------------------------------------------------------------------------------------------
void SWorldCollision::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collStorage = g_ecs->GetArrayStorage<CCollision>();
    auto& scWorld = g_ecs->GetSingleton<SCWorld>();

    for (auto it = g_ecs->Iterate<CTransform, CCollision, CRender>(context); it.IsValid(); ++it)
    {
        CTransform& trans = transStorage[it];
        CCollision& coll = *collStorage.Get(it);

        Vec2& pos = trans.m_pos;
        float& radius = coll.m_radius;

        scWorld.ForEachSolidWorldCoordsOverlappingCapsule(pos, pos, radius, [&scWorld, &pos, &radius](WorldCoords& coords)
        {
            AABB2 tileBounds = scWorld.GetTileBounds(coords);
            GeometryUtils::PushDiscOutOfAABB2D(pos, radius + scWorld.m_worldSettings.m_entityWallBuffer, tileBounds);
            return true;
        });
    }
}