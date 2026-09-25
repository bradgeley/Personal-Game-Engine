// Bradley Christensen - 2022-2026
#include "CCollisionEffect.h"
#include "AbilityComponents.h"



//----------------------------------------------------------------------------------------------------------------------
void CCollisionEffect::Initialize(RolledAoEEffectComponent const& aoeEffectComp)
{
	m_payloadPerSecond = aoeEffectComp.m_payload;
	m_tint = aoeEffectComp.m_renderComp.m_tint;
	m_depth = aoeEffectComp.m_renderComp.m_depth;
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload CCollisionEffect::GetWhileOverlappingPayload(float deltaSeconds) const
{
	HitPayload payload = m_payloadPerSecond;
	payload *= deltaSeconds;
	return payload;
}
