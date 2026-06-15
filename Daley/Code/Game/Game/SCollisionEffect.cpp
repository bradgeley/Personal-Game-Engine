// Bradley Christensen - 2022-2026
#include "SCollisionEffect.h"
#include "CCollisionEffect.h"
#include "CHealth.h"
#include "CTime.h"
#include "HitPayload.h"
#include "SCCollision.h"
#include "SCWorld.h"



//----------------------------------------------------------------------------------------------------------------------
void SCollisionEffect::Startup()
{
    AddReadDependencies<SCCollision, CCollisionEffect>();
	AddWriteDependencies<CHealth, CTime>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionEffect::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCCollision const& scCollision = context.GetSingletonConst<SCCollision>();
	auto const& collisionEffectStorage = context.GetArrayStorageConst<CCollisionEffect>();

	// Write Dependencies
	auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& timeStorage = context.GetArrayStorage<CTime>();
	
    for (OverlapInfo const& overlap : scCollision.m_frameOverlaps)
    {
		EntityID const& entityA = overlap.m_entityA;
		EntityID const& entityB = overlap.m_entityB;

		if (!g_ecs->IsValid(entityA) || !g_ecs->IsValid(entityB))
		{
			continue;
		}

		bool isFirstTouch = scCollision.m_lastFrameOverlaps.find(overlap) == scCollision.m_lastFrameOverlaps.end();

		if (g_ecs->HasComponentUnsafe<CCollisionEffect>(entityA.GetIndex()))
		{
			CCollisionEffect const& aoeA = collisionEffectStorage[entityA.GetIndex()];

			HitPayload payload = isFirstTouch ? aoeA.GetFirstOverlapPayload() : aoeA.GetWhileOverlappingPayload(context.m_deltaSeconds);

			if (payload.IsRelevantToHealth())
			{
				if (g_ecs->HasComponentUnsafe<CHealth>(entityB.GetIndex()))
				{
					CHealth& healthB = healthStorage[entityB.GetIndex()];
					healthB.TakePayload(payload);
				}
			}

			if (payload.IsRelevantToTime())
			{
				if (g_ecs->HasComponentUnsafe<CTime>(entityB.GetIndex()))
				{
					CTime& timeB = timeStorage[entityB.GetIndex()];
					timeB.m_remainingSlowDuration += payload.m_slowDuration;
				}
			}
		}

		if (g_ecs->HasComponentUnsafe<CCollisionEffect>(entityB.GetIndex()))
		{
			CCollisionEffect const& aoeB = collisionEffectStorage[entityB.GetIndex()];

			HitPayload payload = isFirstTouch ? aoeB.GetFirstOverlapPayload() : aoeB.GetWhileOverlappingPayload(context.m_deltaSeconds);

			if (payload.IsRelevantToHealth())
			{
				if (g_ecs->HasComponentUnsafe<CHealth>(entityA.GetIndex()))
				{
					CHealth& healthA = healthStorage[entityA.GetIndex()];
					healthA.TakePayload(payload);
				}
			}

			if (payload.IsRelevantToTime())
			{
				if (g_ecs->HasComponentUnsafe<CTime>(entityA.GetIndex()))
				{
					CTime& timeA = timeStorage[entityA.GetIndex()];
					timeA.m_remainingSlowDuration += payload.m_slowDuration;
				}
			}
		}
    }
}