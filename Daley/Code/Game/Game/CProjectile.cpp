// Bradley Christensen - 2022-2026
#include "CProjectile.h"
#include "EntityDebugContext.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CProjectile::CProjectile(void const*)
{

}



//----------------------------------------------------------------------------------------------------------------------
HitPayload CProjectile::GetMainTargetPayload() const
{
	return m_onHitComp.has_value() ? m_onHitComp->m_payload : HitPayload();
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload CProjectile::GetAoeTargetPayload() const
{
	return m_onHitComp.has_value() && m_onHitComp->m_aoeHitOnHit.has_value() ? m_onHitComp->m_aoeHitOnHit->m_payload : HitPayload();
}



//----------------------------------------------------------------------------------------------------------------------
void CProjectile::AppendDebugString(EntityDebugContext& debugContext) const
{
	debugContext.m_debugString += StringUtils::StringF("Proj Speed: %.1f\n", m_projSpeed);
	if (m_onHitComp.has_value())
	{
		debugContext.m_debugString += StringUtils::StringF("Main Target Payload:\n");
		m_onHitComp->m_payload.AppendDebugString(debugContext);

		if (m_onHitComp->m_aoeHitOnHit.has_value())
		{
			debugContext.m_debugString += StringUtils::StringF("AoE Target Payload:\n");
			m_onHitComp->m_aoeHitOnHit->m_payload.AppendDebugString(debugContext);
		}

		if (m_onHitComp->m_aoeEffectOnHit.has_value())
		{
			m_onHitComp->m_aoeEffectOnHit->AppendDebugString(debugContext);
		}
	}
}