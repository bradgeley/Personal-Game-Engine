// Bradley Christensen - 2022-2026
#include "NameTable.h"
#include "Name.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include <shared_mutex>



//----------------------------------------------------------------------------------------------------------------------
// THE Name Table
//
NameTable* g_nameTable = nullptr;



//----------------------------------------------------------------------------------------------------------------------
NameTable::NameTable()
{
	m_nameTable.emplace_back(Name::s_invalidNameString);
	m_lookupTable.emplace(Name::s_invalidNameString, Name::s_invalidNameIndex);
}



//----------------------------------------------------------------------------------------------------------------------
NameTable& NameTable::Get()
{
	static NameTable* s_instance = [] { g_nameTable = new NameTable(); return g_nameTable; }();
	if (s_instance)
	{
		return *s_instance;
	}
	else
	{
		ERROR_AND_DIE("NameTable::Get() - NameTable instance is null.");
	}
}



//----------------------------------------------------------------------------------------------------------------------
void NameTable::Shutdown()
{
	std::unique_lock<std::shared_mutex> writeLock(m_lookupTableMutex);

	m_nameTable.clear();
	m_lookupTable.clear();

	// Delete the singleton instance, which cannot be recovered
	delete this;
}



//----------------------------------------------------------------------------------------------------------------------
void NameTable::AppendDebugString(std::string& out) const
{
	std::shared_lock<std::shared_mutex> readLock(m_lookupTableMutex);

	for (size_t i = 0; i < m_nameTable.size(); ++i)
	{
		out += StringUtils::StringF("%zu -> %s\n", i, m_nameTable[i].c_str());
	}
	out += StringUtils::StringF("Total Names: %zu\n", m_nameTable.size());
}
