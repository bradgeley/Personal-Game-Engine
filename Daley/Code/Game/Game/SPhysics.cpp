// Bradley Christensen - 2022-2025
#include "SPhysics.h"
#include "CMovement.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "WorldRaycast.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Startup()
{
    AddWriteDependencies<CMovement, CTransform>();
    AddReadDependencies<SCWorld>();
}



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Run(SystemContext const& context)
{
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& world = g_ecs->GetSingleton<SCWorld>();

    for (auto it = g_ecs->Iterate<CMovement, CTransform>(context); it.IsValid(); ++it)
    {
        CMovement& move = moveStorage[it];
        CTransform& transform = transStorage[it];

        Vec2 frameMovement = move.m_frameMovement;

        WorldRaycast raycast;
        raycast.m_tileTagQuery.m_tagsToQuery = (uint8_t) TileTag::IsPath;
        raycast.m_tileTagQuery.m_queryOp = TagQueryOp::DoesNotHaveAll;

        constexpr int maxBounces = 1;
        for (int bounce = 0; bounce <= maxBounces; ++bounce)
        {
            raycast.m_start = transform.m_pos;
            raycast.m_direction = frameMovement.GetNormalizedAndReturnLength(raycast.m_maxDistance);

            WorldRaycastResult result = Raycast(world, raycast);
            if (result.m_blockingHit)
            {
                transform.m_pos = result.m_hitLocation + result.m_hitNormal * StaticWorldSettings::s_collisionEntityWallBuffer;
                Vec2 lostMomentum = frameMovement.GetProjectedOntoNormal(result.m_hitNormal);
                frameMovement -= lostMomentum;
            }
            else
            {
                transform.m_pos += frameMovement;
                break;
            }
        }
		
        move.m_frameMovement = Vec2::ZeroVector;
    }
}

 

//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Shutdown()
{

}
