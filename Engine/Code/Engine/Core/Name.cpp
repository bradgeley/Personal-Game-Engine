// Bradley Christensen - 2022-2026
#include "Name.h"
#include "NameTable.h"
#include <shared_mutex>



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
	NameTable& nameTable = NameTable::Get();

    {
        // Fast path: any number of threads can read the name table at once if the names already exist
        std::shared_lock<std::shared_mutex> readLock(nameTable.m_lookupTableMutex);
        auto const it = nameTable.m_lookupTable.find(string);
        if (it != nameTable.m_lookupTable.end())
        {
            m_nameIndex = static_cast<uint32_t>(it->second);
            return;
        }
    }

	// Slow path: Name needs to be added to the name table
    std::unique_lock<std::shared_mutex> writeLock(nameTable.m_lookupTableMutex);

    auto const it = nameTable.m_lookupTable.find(string); // Re-check: another thread may have inserted it
    if (it != nameTable.m_lookupTable.end())
    {
        m_nameIndex = static_cast<uint32_t>(it->second);
        return;
    }

    m_nameIndex = static_cast<uint32_t>(nameTable.m_nameTable.size());
    nameTable.m_lookupTable[string] = m_nameIndex;
    nameTable.m_nameTable.push_back(string);
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
    if (IsValid())
    {
        std::shared_lock<std::shared_mutex> readLock(NameTable::Get().m_lookupTableMutex);
        std::string const& result = NameTable::Get().m_nameTable[m_nameIndex];
        return result;
    }
    return s_invalidNameString;
}



//----------------------------------------------------------------------------------------------------------------------
const char* Name::ToCStr() const
{
    if (IsValid())
    {
        std::shared_lock<std::shared_mutex> readLock(NameTable::Get().m_lookupTableMutex);
        std::string const& result = NameTable::Get().m_nameTable[m_nameIndex];
        return result.c_str();
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