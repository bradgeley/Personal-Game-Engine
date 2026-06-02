// Bradley Christensen - 2022-2026
#include "CCollisionEffect.h"



//----------------------------------------------------------------------------------------------------------------------
HitPayload CCollisionEffect::GetFirstTouchPayload() const
{
	HitPayload payload;
	payload.m_damage = m_firstOverlapDamage;
	payload.m_burn = m_firstOverlapBurn;
	payload.m_poison = m_firstOverlapPoison;
	payload.m_slowDuration = m_firstOverlapSlow;
	return payload;
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload CCollisionEffect::GetWhileTouchingPayload(float deltaSeconds) const
{
	HitPayload payload;
	payload.m_damage = m_damagePerSecond * deltaSeconds;
	payload.m_burn = m_burnPerSecond * deltaSeconds;
	payload.m_poison = m_poisonPerSecond * deltaSeconds;
	payload.m_slowDuration = m_slowPerSecond * deltaSeconds;
	return payload;
}
