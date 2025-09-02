// Bradley Christensen - 2022-2025
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
#include "Engine/Renderer/Renderer.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Debug/DevConsole.h"



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Startup()
{
    AddWriteDependencies<CMovement, CTransform>();
    AddWriteDependencies<SCDebug, Renderer>();
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
        CMovement& move = moveStorage[it];
        CTransform& transform = transStorage[it];
        const CCollision& collision = collisionStorage[it];
        const float& radius = collision.m_radius;

        // This is how much movement we want to do this frame, total
        Vec2 originalFrameMovement = move.m_frameMovement;
        Vec2& frameMovement = move.m_frameMovement;

        constexpr int maxNumBounces = 3;
        int numBounces = 0;
        while (!frameMovement.IsNearlyZero(0.000001f) && numBounces < maxNumBounces)
        {
            WorldDiscCast discCast;
            discCast.m_start = transform.m_pos;
            discCast.m_direction = frameMovement.GetNormalized();
            discCast.m_maxDistance = frameMovement.GetLength();
            discCast.m_discRadius = radius;

            WorldDiscCastResult result;
            result = DiscCast(scWorld, discCast);

            if (scDebug.m_debugRenderPreventativePhysicsRaycasts) 
            {
				AddVertsForDiscCast(*g_renderer->GetVertexBuffer(scDebug.m_frameUntexVerts), result, 1.f);
            }

            if (result.m_immediateHit)
            {
                GeometryUtils::PushDiscOutOfPoint2D(transform.m_pos, radius, result.m_hitLocation);
                break;
            }
            else if (result.m_blockingHit)
            {
                transform.m_pos = result.m_newDiscCenter;
                transform.m_pos -= discCast.m_direction * scWorld.m_worldSettings.m_entityWallBuffer;
                Vec2 lostMomentum = frameMovement.GetProjectedOntoNormal(result.m_hitNormal);
                frameMovement -= lostMomentum;
                numBounces++;

                if (frameMovement.Dot(originalFrameMovement) < -0.001f)
                {
                    // Don't allow multiple bounces to change the direction we were originally going
                    frameMovement = Vec2::ZeroVector;
                }
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
