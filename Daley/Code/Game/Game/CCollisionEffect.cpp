// Bradley Christensen - 2022-2026
#include "CCollisionEffect.h"
#include "Ability.h"



//----------------------------------------------------------------------------------------------------------------------
void CCollisionEffect::InitializeFromAoEEffect(AbilityAoEEffectComponent const& aoeEffectComp)
{
	m_damagePerSecond = aoeEffectComp.m_damagePerSecond.GetMaxDamage();
	m_burnPerSecond = aoeEffectComp.m_burnPerSecond.GetBurn();
	m_poisonPerSecond = aoeEffectComp.m_poisonPerSecond.GetPoison();
	m_slowPerSecond = aoeEffectComp.m_slowPerSecond.GetDuration();
	m_hastePerSecond = aoeEffectComp.m_hastePerSecond.GetDuration();

    m_tint = aoeEffectComp.m_renderComp.m_tint;
    m_depth = aoeEffectComp.m_renderComp.m_depth;
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload CCollisionEffect::GetWhileOverlappingPayload(float deltaSeconds) const
{
	HitPayload payload;
	payload.m_damage = m_damagePerSecond * deltaSeconds;
	payload.m_burn = m_burnPerSecond * deltaSeconds;
	payload.m_poison = m_poisonPerSecond * deltaSeconds;
	payload.m_slowDuration = m_slowPerSecond * deltaSeconds;
	payload.m_hasteDuration = m_hastePerSecond * deltaSeconds;
	return payload;
}
