// Bradley Christensen - 2022-2026
#include "SCollisionEffect.h"
#include "CAoEEffect.h"
#include "CHealth.h"
#include "HitPayload.h"
#include "SCCollision.h"
#include "SCWorld.h"



//----------------------------------------------------------------------------------------------------------------------
void SCollisionEffect::Startup()
{
    AddReadDependencies<SCCollision, CAoEEffect, CHealth>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCollisionEffect::Run(SystemContext const& context)
{
    SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();
	
    for (OverlapInfo const& overlap : scCollision.m_frameOverlaps)
    {
		EntityID const& entityA = overlap.m_entityA;
		EntityID const& entityB = overlap.m_entityB;

		if (CAoEEffect* aoeA = g_ecs->GetComponent<CAoEEffect>(entityA))
		{
			if (CHealth* healthB = g_ecs->GetComponent<CHealth>(entityB))
			{
				HitPayload payload;
				payload.m_damage = aoeA->m_damagePerSecond * context.m_deltaSeconds;
				payload.m_burn = aoeA->m_burnPerSecond * context.m_deltaSeconds;
				payload.m_poison = aoeA->m_poisonPerSecond * context.m_deltaSeconds;
				healthB->TakePayload(payload);
			}
		}

		if (CAoEEffect* aoeB = g_ecs->GetComponent<CAoEEffect>(entityB))
		{
			if (CHealth* healthA = g_ecs->GetComponent<CHealth>(entityA))
			{
				HitPayload payload;
				payload.m_damage = aoeB->m_damagePerSecond * context.m_deltaSeconds;
				payload.m_burn = aoeB->m_burnPerSecond * context.m_deltaSeconds;
				payload.m_poison = aoeB->m_poisonPerSecond * context.m_deltaSeconds;
				healthA->TakePayload(payload);
			}
		}
    }
}