﻿// Bradley Christensen - 2022-2023
#include "Engine/DataStructures/NamedProperties.h"



//----------------------------------------------------------------------------------------------------------------------
NamedProperties::~NamedProperties()
{
    Clear();
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
bool NamedProperties::Contains(std::string const& name) const
{
    return m_properties.find(name) != m_properties.end();
}
