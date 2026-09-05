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
	return m_onHitComp.m_payload;
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload CProjectile::GetAoeTargetPayload() const
{
	return m_onHitComp.m_aoeHitOnHit.m_payload;
}



//----------------------------------------------------------------------------------------------------------------------
void CProjectile::AppendDebugString(EntityDebugContext& debugContext) const
{
	debugContext.m_debugString += StringUtils::StringF("TargetID: %d\n", m_targetID);
	debugContext.m_debugString += StringUtils::StringF("Proj Speed: %.1f\n", m_projSpeed);

	m_onHitComp.AppendDebugString(debugContext);
}
