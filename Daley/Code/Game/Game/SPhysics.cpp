// Bradley Christensen - 2022-2026
#include "SPhysics.h"
#include "CMovement.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "WorldRaycast.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Startup()
{
    AddReadDependencies<SCWorld>();
    AddWriteDependencies<CMovement, CTransform>();

    m_runWhilePaused = false;

	int numThreads = (int) std::thread::hardware_concurrency();
    if (numThreads > 1)
    {
        m_systemSplittingNumJobs = numThreads - 1; // Leave one thread for the main thread to run other systems on
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Run(SystemContext const& context) const
{
    // Read Dependencies
	SCWorld const& world = context.GetSingletonConst<SCWorld>();

	// Write Dependencies
    auto& moveStorage = context.GetArrayStorage<CMovement>();
    auto& transStorage = context.GetArrayStorage<CTransform>();

    TagQuery isWalkableTagQuery;
	isWalkableTagQuery.m_doesNotHaveAnyTags |= (uint8_t) TileTag::Solid;
	isWalkableTagQuery.m_hasAnyTags |= (uint8_t) TileTag::IsPath;
	isWalkableTagQuery.m_hasAnyTags |= (uint8_t) TileTag::IsGoal;

    for (auto it = context.Iterate<CMovement, CTransform>(); it.IsValid(); ++it)
    {
        CMovement& move = moveStorage[it];
        CTransform& transform = transStorage[it];

        if (!move.GetIsConstrainedToPath())
        {
            transform.m_pos += move.m_frameMovement;
            move.m_frameMovement = Vec2::ZeroVector;
            continue;
		}

        if (!world.DoesTileMatchTagQuery(transform.m_pos, isWalkableTagQuery))
        {
            float nearestPathTileDistanceSquared = FLT_MAX;
            Vec2 nearestPathTileCenter = transform.m_pos;
            world.ForEachPlayableTileOverlappingCircle(transform.m_pos, StaticWorldSettings::s_pathTileSnapRadius, isWalkableTagQuery, [&world, &transform, &nearestPathTileDistanceSquared, &nearestPathTileCenter](IntVec2 const& pathTileCoords)
            {
                Vec2 pathTileCenter = world.GetTileCenter(pathTileCoords);
                float distanceSquared = transform.m_pos.GetDistanceSquaredTo(pathTileCenter);
                if (distanceSquared < nearestPathTileDistanceSquared)
                {
                    nearestPathTileDistanceSquared = distanceSquared;
                    nearestPathTileCenter = pathTileCenter;
                }
                return true;
			});
			transform.m_pos = nearestPathTileCenter;
        }

        Vec2 frameMovement = move.m_frameMovement;
		float distanceRemaining = frameMovement.GetLength();

        WorldRaycast raycast;
        raycast.m_allowedTiles = isWalkableTagQuery;

        constexpr int maxNumBounces = 1;
        int numBounces = 0;
        while (!MathUtils::IsNearlyZero(distanceRemaining) && numBounces <= maxNumBounces)
        {
            raycast.m_start = transform.m_pos;
            raycast.m_direction = frameMovement / distanceRemaining;
            raycast.m_maxDistance = distanceRemaining;

            WorldRaycastResult result = Raycast(world, raycast);
            if (result.m_blockingHit)
            {
                numBounces++;

                transform.m_pos = result.m_hitLocation + result.m_hitNormal * StaticWorldSettings::s_collisionEntityWallBuffer;

                // Zero out movement towards the hit wall
                Vec2 lostMomentum = frameMovement.GetProjectedOntoNormal(result.m_hitNormal);
                frameMovement -= lostMomentum;

                // Clamp frame movement to the remaining distance
                float distanceTraveled = result.m_distance;
				distanceRemaining -= distanceTraveled;
                frameMovement.ClampLength(distanceRemaining);
            }
            else
            {
                Vec2 wouldBePosition = transform.m_pos + frameMovement;
                if (!world.DoesTileMatchTagQuery(wouldBePosition, isWalkableTagQuery))
                {
                    break;
                }
                transform.m_pos = wouldBePosition;
                break;
            }
        }
		
        move.m_frameMovement = Vec2::ZeroVector;
    }
}