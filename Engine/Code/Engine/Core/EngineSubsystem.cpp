// Bradley Christensen - 2022-2023
#include "EngineSubsystem.h"



bool EngineSubsystem::IsEnabled() const
{
    return m_isEnabled;
}



void EngineSubsystem::SetEnabled(bool isEnabled)
{
    m_isEnabled = isEnabled;
    OnSetEnabled(isEnabled);
}
