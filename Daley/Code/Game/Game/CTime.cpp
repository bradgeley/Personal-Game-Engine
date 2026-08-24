// Bradley Christensen - 2022-2026
#include "CTime.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
bool CTime::IsSlowed() const
{
	return m_remainingSlowDuration > 0.f;
}



//----------------------------------------------------------------------------------------------------------------------
bool CTime::IsHasted() const
{
	return m_remainingHasteDuration > 0.f;
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