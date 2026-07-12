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

	BitMask collisionBit = context.GetComponentBitMask<CCollisionEffect>();
	BitMask healthBit = context.GetComponentBitMask<CHealth>();
	BitMask timeBit = context.GetComponentBitMask<CTime>();
	
    for (OverlapInfo const& overlap : scCollision.m_frameOverlaps)
    {
		EntityID const& entityA = overlap.m_entityA;
		EntityID const& entityB = overlap.m_entityB;

		if (!context.IsValid(entityA) || !context.IsValid(entityB))
		{
			continue;
		}

		if (context.HasComponentsUnsafe(entityA.GetIndex(), collisionBit))
		{
			CCollisionEffect const& aoeA = collisionEffectStorage[entityA];

			HitPayload payload = aoeA.GetWhileOverlappingPayload(context.m_deltaSeconds);

			if (payload.IsRelevantToHealth())
			{
				if (context.HasComponentsUnsafe(entityB.GetIndex(), healthBit))
				{
					CHealth& healthB = healthStorage[entityB];
					healthB.TakePayload(payload);
				}
			}

			if (payload.IsRelevantToTime())
			{
				if (context.HasComponentsUnsafe(entityB.GetIndex(), timeBit))
				{
					CTime& timeB = timeStorage[entityB];
					timeB.m_remainingSlowDuration += payload.m_slowDuration;
				}
			}
		}

		if (context.HasComponentsUnsafe(entityB.GetIndex(), collisionBit))
		{
			CCollisionEffect const& aoeB = collisionEffectStorage[entityB];

			HitPayload payload = aoeB.GetWhileOverlappingPayload(context.m_deltaSeconds);

			if (payload.IsRelevantToHealth())
			{
				if (context.HasComponentsUnsafe(entityA.GetIndex(), healthBit))
				{
					CHealth& healthA = healthStorage[entityA];
					healthA.TakePayload(payload);
				}
			}

			if (payload.IsRelevantToTime())
			{
				if (context.HasComponentsUnsafe(entityA.GetIndex(), timeBit))
				{
					CTime& timeA = timeStorage[entityA];
					timeA.m_remainingSlowDuration += payload.m_slowDuration;
				}
			}
		}
    }
}