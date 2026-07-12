// Bradley Christensen - 2022-2026
#include "CCollisionEffect.h"
#include "Ability.h"



//----------------------------------------------------------------------------------------------------------------------
void CCollisionEffect::InitializeFromAoEEffect(AbilityAoEEffectComponent const& aoeEffectComp)
{
    if (aoeEffectComp.m_damagePerSecond.has_value())
    {
        m_damagePerSecond = aoeEffectComp.m_damagePerSecond->m_maxDamage;
    }
    if (aoeEffectComp.m_burnPerSecond.has_value())
    {
        m_burnPerSecond = aoeEffectComp.m_burnPerSecond->m_burn;
    }
    if (aoeEffectComp.m_poisonPerSecond.has_value())
    {
        m_poisonPerSecond = aoeEffectComp.m_poisonPerSecond->m_poison;
    }
    if (aoeEffectComp.m_slowPerSecond.has_value())
    {
        m_slowPerSecond = aoeEffectComp.m_slowPerSecond->m_duration;
    }
    m_shouldRender = aoeEffectComp.m_renderComp.has_value();
    if (aoeEffectComp.m_renderComp.has_value())
    {
        m_tint = aoeEffectComp.m_renderComp->m_tint;
        m_depth = aoeEffectComp.m_renderComp->m_depth;
    }
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload CCollisionEffect::GetWhileOverlappingPayload(float deltaSeconds) const
{
	HitPayload payload;
	payload.m_damage = m_damagePerSecond * deltaSeconds;
	payload.m_burn = m_burnPerSecond * deltaSeconds;
	payload.m_poison = m_poisonPerSecond * deltaSeconds;
	payload.m_slowDuration = m_slowPerSecond * deltaSeconds;
	return payload;
}
