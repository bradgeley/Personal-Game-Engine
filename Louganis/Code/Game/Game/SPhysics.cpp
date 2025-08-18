// Bradley Christensen - 2025
#include "SPhysics.h"
#include "CCollision.h"
#include "CMovement.h"
#include "CTransform.h"
#include "Engine/Math/MathUtils.h"
#include "SMovement.h"
#include "WorldRaycast.h"
#include "SCWorld.h"
#include "SCDebug.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Math/GeometryUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Startup()
{
    AddWriteDependencies<CMovement, CTransform>();
    AddWriteDependencies<SCDebug>();
    AddReadDependencies<CCollision, SCWorld>();

    g_eventSystem->SubscribeMethod("DebugRenderPreventativePhysics", this, &SPhysics::DebugRenderPreventativePhysics);
}



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Run(SystemContext const& context)
{
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collisionStorage = g_ecs->GetArrayStorage<CCollision>();
    auto& scWorld = g_ecs->GetSingleton<SCWorld>();
    auto& scDebug = g_ecs->GetSingleton<SCDebug>();

    for (auto it = g_ecs->Iterate<CMovement, CTransform>(context); it.IsValid(); ++it)
    {
        EntityID& ent = it.m_currentIndex;
        CMovement& move = moveStorage[ent];
        CTransform& transform = transStorage[ent];
        const CCollision& collision = collisionStorage[ent];
        const float& radius = collision.m_radius;

        // This is how much movement we want to do this frame, total
        Vec2& frameMovement = move.m_frameMovement;

        while (!frameMovement.IsNearlyZero(scWorld.m_worldSettings.m_entityMinimumMovement))
        {
            WorldDiscCast discCast;
            discCast.m_start = transform.m_pos;
            discCast.m_direction = frameMovement.GetNormalized();
            discCast.m_maxDistance = frameMovement.GetLength();
            discCast.m_discRadius = radius;
            WorldDiscCastResult result = DiscCast(scWorld, discCast);
            AddVertsForDiscCast(scDebug.m_frameVerts, result);

            if (result.m_immediateHit)
            {
                transform.m_pos += frameMovement;
                frameMovement = Vec2::ZeroVector;
            }
            else if (result.m_blockingHit)
            {
                if (MathUtils::IsNearlyEqual(result.m_t, 0.f, 0.000001f))
                {
                    frameMovement = Vec2::ZeroVector;
                    break;
                }
                Vec2 actualMovement = result.m_newDiscCenter - transform.m_pos;
                transform.m_pos = result.m_newDiscCenter;

                frameMovement -= actualMovement;
                frameMovement = frameMovement.GetProjectedOntoNormal(result.m_hitNormal.GetRotated90());
            }
            else
            {
                transform.m_pos = result.m_newDiscCenter;
                frameMovement = Vec2::ZeroVector;
            }
        }
    }
}

 

//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Shutdown()
{
    g_eventSystem->UnsubscribeMethod("DebugRenderPreventativePhysics", this, &SPhysics::DebugRenderPreventativePhysics);
}



//----------------------------------------------------------------------------------------------------------------------
bool SPhysics::DebugRenderPreventativePhysics(NamedProperties&)
{
    auto& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderPreventativePhysicsRaycasts = !scDebug.m_debugRenderPreventativePhysicsRaycasts;
    return false;
}
