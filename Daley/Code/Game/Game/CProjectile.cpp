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
	return m_onHitComp.has_value() ? m_onHitComp->GetPayload() : HitPayload();
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload CProjectile::GetAoeTargetPayload() const
{
	return m_onHitComp.has_value() && m_onHitComp->m_aoeHitOnHit.has_value() ? m_onHitComp->m_aoeHitOnHit->GetPayload() : HitPayload();
}



//----------------------------------------------------------------------------------------------------------------------
void CProjectile::AppendDebugString(std::string& out_string) const
{
	out_string += StringUtils::StringF("Proj Speed: %.1f\n", m_projSpeed);
	if (m_critComp.has_value())
	{
		m_critComp->AppendDebugString(out_string);
	}
	if (m_onHitComp.has_value())
	{
		m_onHitComp->AppendDebugString(out_string);
	}
}