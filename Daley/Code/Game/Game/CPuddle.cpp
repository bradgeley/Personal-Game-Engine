// Bradley Christensen - 2022-2026
#include "CPuddle.h"
#include "AbilityComponents.h"



//----------------------------------------------------------------------------------------------------------------------
void CPuddle::Initialize(RolledPuddleComponent const& puddleComp)
{
	m_payloadPerSecond = puddleComp.m_payload;
	m_tint = puddleComp.m_renderComp.m_tint;
	m_depth = puddleComp.m_renderComp.m_depth;
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload CPuddle::GetWhileOverlappingPayload(float deltaSeconds) const
{
	HitPayload payload = m_payloadPerSecond;
	payload *= deltaSeconds;
	return payload;
}
