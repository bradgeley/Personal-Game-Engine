// Bradley Christensen - 2022-2026
#include "CProjectile.h"
#include "EntityDebugContext.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CProjectile::CProjectile(void const*)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool CProjectile::NextTarget()
{
	if (m_currentTargetIndex + 1 >= m_numChains)
	{
		return false;
	}
	m_currentTargetIndex++;
	return true;
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
	for (int i = 0; i < StaticGameSettings::s_maxChainTargets; ++i)
	{
		debugContext.m_debugString += StringUtils::StringF("Target %d: %d\n", i, m_targets[i]);
	}
	debugContext.m_debugString += StringUtils::StringF("Proj Speed: %.1f\n", m_projSpeed);

	m_onHitComp.AppendDebugString(debugContext);
}
