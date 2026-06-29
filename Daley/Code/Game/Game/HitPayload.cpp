// Bradley Christensen - 2022-2026
#include "HitPayload.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void HitPayload::AppendDebugString(std::string& out_string) const
{
	if (m_didCrit)
	{
		out_string += "Critical Hit!\n";
	}
	if (m_damage > 0.f)
	{
		out_string += StringUtils::StringF("Damage: %.2f\n", m_damage);
	}
	if (m_burn > 0.f)
	{
		out_string += StringUtils::StringF("Burn: %.2f\n", m_burn);
	}
	if (m_poison > 0.f)
	{
		out_string += StringUtils::StringF("Poison: %.2f\n", m_poison);
	}
	if (m_slowDuration > 0.f)
	{
		out_string += StringUtils::StringF("Slow Duration: %.2f\n", m_slowDuration);
	}
}