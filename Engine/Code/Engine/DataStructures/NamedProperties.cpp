// Bradley Christensen - 2022-2025
#include "Engine/DataStructures/NamedProperties.h"



//----------------------------------------------------------------------------------------------------------------------
NamedProperties::~NamedProperties()
{
    Clear();
}



//----------------------------------------------------------------------------------------------------------------------
std::string NamedProperties::Get(Name key, const char* defaultValue) const
{
    return Get<std::string>(key, std::string(defaultValue));
}



//----------------------------------------------------------------------------------------------------------------------
void NamedProperties::Clear()
{
    for (auto& prop : m_properties)
    {
        delete prop.second;
    }
    m_properties.clear();
}



//----------------------------------------------------------------------------------------------------------------------
int NamedProperties::Size() const
{
    return (int) m_properties.size();
}



//----------------------------------------------------------------------------------------------------------------------
bool NamedProperties::Contains(Name name) const
{
    return m_properties.find(name) != m_properties.end();
}
