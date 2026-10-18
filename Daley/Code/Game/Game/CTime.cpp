// Bradley Christensen - 2022-2026
#include "CTime.h"
#include "HitPayload.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
bool CTime::IsSlowed() const
{
	return m_remainingSlowDuration > 0.f || GetIsSlowedThisFrame();
}



//----------------------------------------------------------------------------------------------------------------------
bool CTime::IsHasted() const
{
	return m_remainingHasteDuration > 0.f || GetIsHastedThisFrame();
}



//----------------------------------------------------------------------------------------------------------------------
void CTime::TakePayload(HitPayload const& payload)
{
	float slowTaken = payload.m_slowDuration * (1.f - m_slowResistance);
	m_remainingSlowDuration += slowTaken;
	m_remainingHasteDuration += payload.m_hasteDuration;
}



//----------------------------------------------------------------------------------------------------------------------
bool CTime::GetIsSlowedThisFrame() const
{
	return (static_cast<uint8_t>(m_timeFlags) & static_cast<uint8_t>(TimeFlags::SlowedThisFrame)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CTime::GetIsHastedThisFrame() const
{
	return (static_cast<uint8_t>(m_timeFlags) & static_cast<uint8_t>(TimeFlags::HastedThisFrame)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
void CTime::SetIsSlowedThisFrame(bool isSlowed)
{
	if (isSlowed)
	{
		m_timeFlags = static_cast<TimeFlags>(static_cast<uint8_t>(m_timeFlags) | static_cast<uint8_t>(TimeFlags::SlowedThisFrame));
	}
	else
	{
		m_timeFlags = static_cast<TimeFlags>(static_cast<uint8_t>(m_timeFlags) & ~static_cast<uint8_t>(TimeFlags::SlowedThisFrame));
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CTime::SetIsHastedThisFrame(bool isHasted)
{
	if (isHasted)
	{
		m_timeFlags = static_cast<TimeFlags>(static_cast<uint8_t>(m_timeFlags) | static_cast<uint8_t>(TimeFlags::HastedThisFrame));
	}
	else
	{
		m_timeFlags = static_cast<TimeFlags>(static_cast<uint8_t>(m_timeFlags) & ~static_cast<uint8_t>(TimeFlags::HastedThisFrame));
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CTime::AppendDebugString(std::string& out_string) const
{
	if (IsSlowed())
	{
		out_string += StringUtils::StringF("Slow: %.1f\n", m_remainingSlowDuration);
	}
	if (IsHasted())
	{
		out_string += StringUtils::StringF("Haste: %.1f\n", m_remainingHasteDuration);
	}
}