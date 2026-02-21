// Bradley Christensen - 2022-2025
#include "SPhysics.h"
#include "CCollision.h"
#include "CMovement.h"
#include "CTransform.h"
#include "WorldRaycast.h"
#include "SCWorld.h"
#include "SCDebug.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Startup()
{
    AddWriteDependencies<CMovement, CTransform>();
    AddWriteDependencies<Renderer>();
    AddReadDependencies<CCollision, SCDebug, SCWorld>();

	DevConsoleUtils::AddDevConsoleCommand("DebugRenderPreventativePhysics", &SPhysics::DebugRenderPreventativePhysics);

    int numThreads = std::thread::hardware_concurrency() - 1;
    m_systemSplittingNumJobs = numThreads - 1;
}



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Run(SystemContext const& context)
{
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collisionStorage = g_ecs->GetArrayStorage<CCollision>();
    SCWorld const& scWorld = g_ecs->GetSingleton<SCWorld>();
    SCDebug const& scDebug = g_ecs->GetSingleton<SCDebug>();

    for (auto it = g_ecs->Iterate<CMovement, CTransform, CCollision>(context); it.IsValid(); ++it)
    {
        CCollision const& collision = collisionStorage[it];
        CMovement& move = moveStorage[it];
        CTransform& transform = transStorage[it];
        if (collision.IsCollisionEnabled() == false)
        {
			transform.m_pos += move.m_frameMovement;
			move.m_frameMovement = Vec2::ZeroVector;
            continue;
		}

        const float& radius = collision.m_radius;

		Vec2 collisionPos = transform.m_pos + collision.m_offset;

        // This is how much movement we want to do this frame, total
        Vec2 originalFrameMovement = move.m_frameMovement;
        Vec2& frameMovement = move.m_frameMovement;

        constexpr int maxNumBounces = 3;
        int numBounces = 0;
        while (!frameMovement.IsNearlyZero(0.000001f) && numBounces < maxNumBounces)
        {
            WorldDiscCast discCast;
            discCast.m_start = collisionPos;
            discCast.m_direction = frameMovement.GetNormalized();
            discCast.m_maxDistance = frameMovement.GetLength();
            discCast.m_discRadius = radius;

            WorldDiscCastResult result;
            result = DiscCast(scWorld, discCast);

            if (scDebug.m_debugRenderPreventativePhysics) 
            {
				AddVertsForDiscCast(*g_renderer->GetVertexBuffer(scDebug.m_frameUntexVerts), result, 1.f);
            }

            if (result.m_immediateHit)
            {
                GeometryUtils::PushDiscOutOfPoint2D(collisionPos, radius + scWorld.m_worldSettings.m_entityWallBuffer, result.m_hitLocation);
				transform.m_pos = collisionPos - collision.m_offset;
                break;
            }
            else if (result.m_blockingHit)
            {
                collisionPos = result.m_newDiscCenter;
                collisionPos -= discCast.m_direction * scWorld.m_worldSettings.m_entityWallBuffer;
				transform.m_pos = collisionPos - collision.m_offset;
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
	DevConsoleUtils::RemoveDevConsoleCommand("DebugRenderPreventativePhysics", &SPhysics::DebugRenderPreventativePhysics);
}



//----------------------------------------------------------------------------------------------------------------------
bool SPhysics::DebugRenderPreventativePhysics(NamedProperties&)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderPreventativePhysics = !scDebug.m_debugRenderPreventativePhysics;
    return false;
}
