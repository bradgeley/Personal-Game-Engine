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
	SCCollision const& scCollision = g_ecs->GetSingleton<SCCollision>();
	
    for (OverlapInfo const& overlap : scCollision.m_frameOverlaps)
    {
		EntityID const& entityA = overlap.m_entityA;
		EntityID const& entityB = overlap.m_entityB;

		bool isFirstTouch = scCollision.m_lastFrameOverlaps.find(overlap) == scCollision.m_lastFrameOverlaps.end();

		if (CCollisionEffect const* aoeA = g_ecs->GetComponent<CCollisionEffect>(entityA))
		{
			HitPayload payload = isFirstTouch ? aoeA->GetFirstTouchPayload() : aoeA->GetWhileTouchingPayload(context.m_deltaSeconds);

			if (payload.IsRelevantToHealth())
			{
				if (CHealth* healthB = g_ecs->GetComponent<CHealth>(entityB))
				{
					healthB->TakePayload(payload);
				}
			}

			if (payload.IsRelevantToTime())
			{
				if (CTime* timeB = g_ecs->GetComponent<CTime>(entityB))
				{
					timeB->m_remainingSlowDuration += payload.m_slowDuration;
				}
			}
		}

		if (CCollisionEffect* aoeB = g_ecs->GetComponent<CCollisionEffect>(entityB))
		{
			HitPayload payload = isFirstTouch ? aoeB->GetFirstTouchPayload() : aoeB->GetWhileTouchingPayload(context.m_deltaSeconds);

			if (payload.IsRelevantToHealth())
			{
				if (CHealth* healthA = g_ecs->GetComponent<CHealth>(entityA))
				{
					healthA->TakePayload(payload);
				}
			}

			if (payload.IsRelevantToTime())
			{
				if (CTime* timeA = g_ecs->GetComponent<CTime>(entityA))
				{
					timeA->m_remainingSlowDuration += payload.m_slowDuration;
				}
			}
		}
    }
}