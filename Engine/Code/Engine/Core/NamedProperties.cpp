// Bradley Christensen - 2022-2026
#include "Engine/Core/NamedProperties.h"



//----------------------------------------------------------------------------------------------------------------------
NamedProperties::~NamedProperties()
{
    Clear();
}



//----------------------------------------------------------------------------------------------------------------------
NamedProperties::NamedProperties(NamedProperties const& other)
{
	*this = other;
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



//----------------------------------------------------------------------------------------------------------------------
void NamedProperties::operator=(NamedProperties const& other)
{
    Clear();

	for (auto& prop : other.m_properties)
	{
		Name name = prop.first;
		PropertyBase* property = prop.second;
		m_properties[name] = property->Clone();
	}
}