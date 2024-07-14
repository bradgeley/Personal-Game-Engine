// Bradley Christensen - 2022-2023
#include "EngineSubsystem.h"



//----------------------------------------------------------------------------------------------------------------------
EngineSubsystem::EngineSubsystem(std::string const& name) : m_name(name)
{
}



//----------------------------------------------------------------------------------------------------------------------
std::string const& EngineSubsystem::GetName()
{
    return m_name;
}



//----------------------------------------------------------------------------------------------------------------------
bool EngineSubsystem::IsEnabled() const
{
    return m_isEnabled;
}



//----------------------------------------------------------------------------------------------------------------------
void EngineSubsystem::SetEnabled(bool isEnabled)
{
    m_isEnabled = isEnabled;
    OnSetEnabled(isEnabled);
}
