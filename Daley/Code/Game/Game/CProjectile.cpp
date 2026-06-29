// Bradley Christensen - 2022-2026
#include "CProjectile.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Math/RandomNumberGenerator.h"



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
void CProjectile::AppendDebugString(std::string& out_string) const
{
	out_string += StringUtils::StringF("Proj Speed: %.1f\n", m_projSpeed);
	if (m_onHitComp.has_value())
	{
		out_string += StringUtils::StringF("Main Target Payload:\n");
		m_onHitComp->m_payload.AppendDebugString(out_string);

		if (m_onHitComp->m_aoeHitOnHit.has_value())
		{
			out_string += StringUtils::StringF("AoE Target Payload:\n");
			m_onHitComp->m_aoeHitOnHit->m_payload.AppendDebugString(out_string);
		}

		if (m_onHitComp->m_aoeEffectOnHit.has_value())
		{
			m_onHitComp->m_aoeEffectOnHit->AppendDebugString(out_string);
		}
	}
}