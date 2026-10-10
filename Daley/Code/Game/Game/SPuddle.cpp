// Bradley Christensen - 2022-2026
#include "SPuddle.h"
#include "CCollision.h"
#include "CHealth.h"
#include "CTime.h"
#include "CTransform.h"
#include "CPuddle.h"
#include "HitPayload.h"
#include "SCWorld.h"
#include "SCCollision.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
void SPuddle::Startup()
{
    AddReadDependencies<SCCollision, CCollision, CPuddle, CTransform, SCWorld>();
	AddWriteDependencies<CHealth, CTime>();

	m_runWhilePaused = false;
}



//----------------------------------------------------------------------------------------------------------------------
void SPuddle::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCCollision const& scCollision = context.GetSingletonConst<SCCollision>();
	SCWorld const& world = context.GetSingletonConst<SCWorld>();
	auto const& puddleStorage = context.GetArrayStorageConst<CPuddle>();
	auto const& transformStorage = context.GetArrayStorageConst<CTransform>();
	auto const& collisionStorage = context.GetArrayStorageConst<CCollision>();

	// Write Dependencies
	auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& timeStorage = context.GetArrayStorage<CTime>();

	for (auto it = context.Iterate<CCollision, CPuddle, CTransform>(); it.IsValid(); ++it)
	{
		CPuddle const& puddle = puddleStorage[it];

		bool isRelevantToHealth = puddle.m_payloadPerSecond.IsRelevantToHealth();
		bool isRelevantToTime = puddle.m_payloadPerSecond.IsRelevantToTime();

		if (!isRelevantToHealth && !isRelevantToTime)
		{
			continue;
		}

		CTransform const& transform = transformStorage[it];
		CCollision const& collision = collisionStorage[it];

		HitPayload framePayload = puddle.GetWhileOverlappingPayload(context.m_deltaSeconds);

		world.ForEachPathTileOverlappingCircle(transform.m_pos, collision.m_radius, [&](IntVec2 const& worldCoords)
		{
			int tileIndex = world.m_tiles.GetIndexForCoords(worldCoords);
			scCollision.ForEachEnemyInTile(tileIndex, [&](EntityID entityID)
			{
				if (isRelevantToHealth)
				{
					CHealth& health = healthStorage[entityID];
					health.TakePayload(framePayload);
				}

				if (isRelevantToTime)
				{
					CTime& time = timeStorage[entityID];
					time.TakePayload(framePayload);
				}

				return true; // keep iterating
			});
			return true; // keep iterating
		});
	}
}