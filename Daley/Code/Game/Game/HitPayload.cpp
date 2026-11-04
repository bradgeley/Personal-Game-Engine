// Bradley Christensen - 2022-2026
#include "HitPayload.h"
#include "EntityDebugContext.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void HitPayload::AppendDebugString(EntityDebugContext& debugContext) const
{
	if (GetDidCrit())
	{
		debugContext.m_debugString += "Critical Hit!\n";
	}
	if (m_physical > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("Damage: %.2f\n", m_physical);
	}
	if (m_burn > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("Burn: %.2f\n", m_burn);
	}
	if (m_poison > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("Poison: %.2f\n", m_poison);
	}
	if (m_vulnerability > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("Vuln: %.2f\n", m_vulnerability);
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



//----------------------------------------------------------------------------------------------------------------------
void HitPayload::SetDidCrit(bool didCrit)
{
	if (didCrit)
	{
		m_flags |= static_cast<uint8_t>(HitPayloadFlags::DidCrit);
	}
	else
	{
		m_flags &= ~static_cast<uint8_t>(HitPayloadFlags::DidCrit);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void HitPayload::SetMultiplyByMaxHealth(bool multiplyByMaxHealth)
{
	if (multiplyByMaxHealth)
	{
		m_flags |= static_cast<uint8_t>(HitPayloadFlags::MultiplyByMaxHealth);
	}
	else
	{
		m_flags &= ~static_cast<uint8_t>(HitPayloadFlags::MultiplyByMaxHealth);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void HitPayload::SetIgnoreResistances(bool ignoreResistances)
{
	if (ignoreResistances)
	{
		m_flags |= static_cast<uint8_t>(HitPayloadFlags::IgnoreResistances);
	}
	else
	{
		m_flags &= ~static_cast<uint8_t>(HitPayloadFlags::IgnoreResistances);
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool HitPayload::GetDidCrit() const
{
	return static_cast<uint8_t>(m_flags) & static_cast<uint8_t>(HitPayloadFlags::DidCrit);
}



//----------------------------------------------------------------------------------------------------------------------
bool HitPayload::GetMultiplyByMaxHealth() const
{
	return static_cast<uint8_t>(m_flags) & static_cast<uint8_t>(HitPayloadFlags::MultiplyByMaxHealth);
}



//----------------------------------------------------------------------------------------------------------------------
bool HitPayload::GetIgnoreResistances() const
{
	return static_cast<uint8_t>(m_flags) & static_cast<uint8_t>(HitPayloadFlags::IgnoreResistances);
}
