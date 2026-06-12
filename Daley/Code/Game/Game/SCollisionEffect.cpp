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
void SCollisionEffect::Run(SystemContext const& context)
{
	// Read Dependencies
	SCCollision const& scCollision = g_ecs->GetSingleton<SCCollision>();
	auto const& collisionEffectStorage = g_ecs->GetArrayStorage<CCollisionEffect>();

	// Write Dependencies
	auto& healthStorage = g_ecs->GetArrayStorage<CHealth>();
	auto& timeStorage = g_ecs->GetArrayStorage<CTime>();
	
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
			CCollisionEffect const& aoeA = *collisionEffectStorage.Get(entityA.GetIndex());

			HitPayload payload = isFirstTouch ? aoeA.GetFirstOverlapPayload() : aoeA.GetWhileOverlappingPayload(context.m_deltaSeconds);

			if (payload.IsRelevantToHealth())
			{
				if (g_ecs->HasComponentUnsafe<CHealth>(entityB.GetIndex()))
				{
					CHealth& healthB = *healthStorage.Get(entityB.GetIndex());
					healthB.TakePayload(payload);
				}
			}

			if (payload.IsRelevantToTime())
			{
				if (g_ecs->HasComponentUnsafe<CTime>(entityB.GetIndex()))
				{
					CTime& timeB = *timeStorage.Get(entityB.GetIndex());
					timeB.m_remainingSlowDuration += payload.m_slowDuration;
				}
			}
		}

		if (g_ecs->HasComponentUnsafe<CCollisionEffect>(entityB.GetIndex()))
		{
			CCollisionEffect const& aoeB = *collisionEffectStorage.Get(entityB.GetIndex());

			HitPayload payload = isFirstTouch ? aoeB.GetFirstOverlapPayload() : aoeB.GetWhileOverlappingPayload(context.m_deltaSeconds);

			if (payload.IsRelevantToHealth())
			{
				if (g_ecs->HasComponentUnsafe<CHealth>(entityA.GetIndex()))
				{
					CHealth& healthA = *healthStorage.Get(entityA.GetIndex());
					healthA.TakePayload(payload);
				}
			}

			if (payload.IsRelevantToTime())
			{
				if (g_ecs->HasComponentUnsafe<CTime>(entityA.GetIndex()))
				{
					CTime& timeA = *timeStorage.Get(entityA.GetIndex());
					timeA.m_remainingSlowDuration += payload.m_slowDuration;
				}
			}
		}
    }
}