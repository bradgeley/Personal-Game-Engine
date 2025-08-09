// Bradley Christensen - 2023
#include "CCollision.h"
#include "CMovement.h"
#include "CTransform.h"
#include "Engine/Math/MathUtils.h"
#include "SMovement.h"
#include "WorldRaycast.h"
#include "SCWorld.h"
#include "SCDebug.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Math/GeometryUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Startup()
{
    AddWriteDependencies<CMovement, CTransform>();
    AddWriteDependencies<SCDebug>();
    AddReadDependencies<CCollision, SCWorld>();
}



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Run(SystemContext const& context)
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

        Vec2 frameMovement = move.m_frameMoveDir * move.m_movementSpeed * context.m_deltaSeconds;

        while (!frameMovement.IsNearlyZero(scWorld.m_worldSettings.m_entityMinimumMovement))
        {
            // Run preventative physics raycasts
            float tOfShortestRaycastHit = 1.f;
            WorldRaycastResult shortestHitResult;

            Vec2 raycastOffsets[4] = { Vec2(0.f, radius), Vec2(0.f, -radius), Vec2(radius, 0.f), Vec2(-radius, 0.f) };
            for (int raycastIndex = 0; raycastIndex < 4; ++raycastIndex)
            {
                Vec2 const& raycastOffset = raycastOffsets[raycastIndex];

                if (DotProduct2D(raycastOffset, frameMovement) <= 0)
                {
                    // Offsets on the opposite side as the frame movement are unnecessary
                    continue;
                }

                float distance = frameMovement.GetLength();

                WorldRaycast raycast;
                raycast.m_direction = frameMovement / distance;
                raycast.m_start = transform.m_pos + raycastOffset;
                raycast.m_maxDistance = distance;
                raycast.m_queryWorldTiles = true;

                if (scDebug.m_debugRenderPreventativePhysicsRaycasts)
                {
                    AddVertsForArrow2D(scDebug.FrameVerts.GetMutableVerts(), raycast.m_start, raycast.m_start + raycast.m_direction * raycast.m_maxDistance, 0.033f, Rgba8::Yellow);
                }

                WorldRaycastResult result = Raycast(scWorld, raycast);
                if (result.m_blockingHit)
                {
                    if (result.m_t < tOfShortestRaycastHit)
                    {
                        tOfShortestRaycastHit = result.m_t;
                        shortestHitResult = result;
                    }
                }
            }

            if (shortestHitResult.m_blockingHit)
            {
                if (scDebug.m_debugRenderPreventativePhysicsRaycasts)
                {
                    AddVertsForArrow2D(scDebug.FrameVerts.GetMutableVerts(), shortestHitResult.m_hitLocation, shortestHitResult.m_hitLocation + shortestHitResult.m_hitNormal * 0.25f, 0.033f, Rgba8::Red);
                }
                transform.m_pos = shortestHitResult.m_hitLocation + shortestHitResult.m_hitNormal * (collision.m_radius + scWorld.m_worldSettings.m_entityWallBuffer);
                frameMovement -= frameMovement * shortestHitResult.m_t; // subtract out the movement we already completed
                // Now subtract out all the movement that was in the direction of the wall we hit
                Vec2 projectedIntoWall = frameMovement.GetProjectedOntoNormal(shortestHitResult.m_hitNormal);
                frameMovement -= projectedIntoWall;
            }
            else if (shortestHitResult.m_immediateHit)
            {
                PushDiscOutOfPoint2D(transform.m_pos, collision.m_radius + scWorld.m_worldSettings.m_entityWallBuffer, shortestHitResult.m_hitLocation);
                frameMovement = Vec2::ZeroVector;
            }
            else
            {
                if (scDebug.m_debugRenderPreventativePhysicsRaycasts)
                {
                    AddVertsForArrow2D(scDebug.FrameVerts.GetMutableVerts(), transform.m_pos, transform.m_pos + frameMovement, 0.033f, Rgba8::Green);
                }
                transform.m_pos += frameMovement;
                frameMovement = Vec2::ZeroVector;
            }
        }
    }
}
