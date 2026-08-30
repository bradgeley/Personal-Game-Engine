// Bradley Christensen - 2022-2026
#include "CCollisionEffect.h"
#include "Ability.h"



//----------------------------------------------------------------------------------------------------------------------
void CCollisionEffect::InitializeFromAoEEffect(AbilityAoEEffectComponent const& aoeEffectComp)
{
    if (aoeEffectComp.m_damagePerSecond.has_value())
    {
        m_damagePerSecond = aoeEffectComp.m_damagePerSecond->GetMaxDamage();
    }
    if (aoeEffectComp.m_burnPerSecond.has_value())
    {
        m_burnPerSecond = aoeEffectComp.m_burnPerSecond->GetBurn();
    }
    if (aoeEffectComp.m_poisonPerSecond.has_value())
    {
        m_poisonPerSecond = aoeEffectComp.m_poisonPerSecond->GetPoison();
    }
    if (aoeEffectComp.m_slowPerSecond.has_value())
    {
        m_slowPerSecond = aoeEffectComp.m_slowPerSecond->GetDuration();
    }
	if (aoeEffectComp.m_hastePerSecond.has_value())
	{
		m_hastePerSecond = aoeEffectComp.m_hastePerSecond->GetDuration();
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
	payload.m_hasteDuration = m_hastePerSecond * deltaSeconds;
	return payload;
}
