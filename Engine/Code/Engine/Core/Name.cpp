// Bradley Christensen - 2022-2025
#include "Name.h"
#include "NameTable.h"
#include "Engine/Core/ErrorUtils.h"
#include <limits>



//----------------------------------------------------------------------------------------------------------------------
Name Name::Invalid = Name((uint32_t) 0);
uint32_t Name::s_invalidNameIndex = 0;
std::string Name::s_invalidNameString = "Invalid Name";



//----------------------------------------------------------------------------------------------------------------------
Name::Name() : Name(s_invalidNameString)
{
}



//----------------------------------------------------------------------------------------------------------------------
Name::Name(std::string const& string)
{
    ASSERT_OR_DIE(g_nameTable, "Name table does not exist yet.");

    #if defined(NAME_USE_DEBUG_STRING)
        m_debugString = string;
    #endif

	// Need to lock a mutex for insertion into the name table, since this could be happening on multiple threads at once
	// After that, the name table itself is a deque, which means that pointers to strings in the table will not be invalidated by future insertions
    // so no mutex lock is needed for comparing names or looking up the corresponding string.
	std::unique_lock<std::mutex> lock(g_nameTable->m_lookupTableMutex);

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
    #if defined(NAME_USE_DEBUG_STRING)
        m_debugString = string;
    #endif
}



//----------------------------------------------------------------------------------------------------------------------
Name::Name(Name const& other) : m_nameIndex(other.m_nameIndex)
{
    #if defined(NAME_USE_DEBUG_STRING)
        m_debugString = other.m_debugString;
    #endif
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
    return s_invalidNameString;
}



//----------------------------------------------------------------------------------------------------------------------
const char* Name::ToCStr() const
{
    if (g_nameTable && IsValid())
    {
        return g_nameTable->m_nameTable[m_nameIndex].c_str();
    }
    return s_invalidNameString.c_str();
}



//----------------------------------------------------------------------------------------------------------------------
bool Name::operator==(Name const& other) const
{
    return m_nameIndex == other.m_nameIndex;
}



//----------------------------------------------------------------------------------------------------------------------
bool Name::operator!=(Name const& other) const
{
    return m_nameIndex != other.m_nameIndex;
}



//----------------------------------------------------------------------------------------------------------------------
Name::Name(uint32_t index) : m_nameIndex(index)
{
}