// Bradley Christensen - 2025
#include "Name.h"
#include "NameTable.h"
#include <limits>



//----------------------------------------------------------------------------------------------------------------------
Name Name::s_invalidName = Name((uint32_t) 0);
uint32_t Name::s_invalidNameIndex = 0;
std::string Name::s_invalidNameString = "Invalid Name";



//----------------------------------------------------------------------------------------------------------------------
Name::Name(std::string const& string)
{
    auto it = g_nameTable->m_lookupTable.find(string);
    if (it == g_nameTable->m_lookupTable.end())
    {
        m_nameIndex = static_cast<uint32_t>(g_nameTable->m_nameTable.size());
        g_nameTable->m_lookupTable[string] = m_nameIndex;
        g_nameTable->m_nameTable.push_back(string);
    }
    else
    {
        m_nameIndex = static_cast<uint32_t>(it->second);
    }
}



//----------------------------------------------------------------------------------------------------------------------
Name::Name(const char* string) : Name(std::string(string))
{
}



//----------------------------------------------------------------------------------------------------------------------
Name::Name(Name const& other) : m_nameIndex(other.m_nameIndex)
{
}



//----------------------------------------------------------------------------------------------------------------------
bool Name::IsValid() const
{
    return m_nameIndex != s_invalidNameIndex;
}



//----------------------------------------------------------------------------------------------------------------------
uint32_t Name::GetNameIndex() const
{
    return m_nameIndex;
}



//----------------------------------------------------------------------------------------------------------------------
std::string const& Name::ToString() const
{
    if (g_nameTable && IsValid())
    {
        return g_nameTable->m_nameTable[m_nameIndex];
    }
    return s_invalidName.ToString();
}



//----------------------------------------------------------------------------------------------------------------------
const char* Name::ToCStr() const
{
    return ToString().c_str();
}



//----------------------------------------------------------------------------------------------------------------------
bool Name::operator==(Name const& other) const
{
    return m_nameIndex == other.m_nameIndex;
}



//----------------------------------------------------------------------------------------------------------------------
Name::Name(uint32_t index) : m_nameIndex(index)
{
}