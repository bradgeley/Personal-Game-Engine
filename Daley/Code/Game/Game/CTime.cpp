// Bradley Christensen - 2022-2026
#include "CTime.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
bool CTime::IsSlowed() const
{
	return m_remainingSlowDuration > 0.f;
}


//----------------------------------------------------------------------------------------------------------------------
void CTime::AppendDebugString(std::string& out_string) const
{
	out_string += StringUtils::StringF("Slow: %.1f\n", m_remainingSlowDuration);
}