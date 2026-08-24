// Bradley Christensen - 2022-2026
#include "HitPayload.h"
#include "EntityDebugContext.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void HitPayload::AppendDebugString(EntityDebugContext& debugContext) const
{
	if (m_didCrit)
	{
		debugContext.m_debugString += "Critical Hit!\n";
	}
	if (m_damage > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("Damage: %.2f\n", m_damage);
	}
	if (m_burn > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("Burn: %.2f\n", m_burn);
	}
	if (m_poison > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("Poison: %.2f\n", m_poison);
	}
	if (m_slowDuration > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("Slow Duration: %.2f\n", m_slowDuration);
	}
	if (m_hasteDuration > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("Haste Duration: %.2f\n", m_hasteDuration);
	}
}