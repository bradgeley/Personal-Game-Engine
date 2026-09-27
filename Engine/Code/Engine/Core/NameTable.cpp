// Bradley Christensen - 2022-2026
#include "NameTable.h"
#include "Name.h"



//----------------------------------------------------------------------------------------------------------------------
// THE Name Table
//
NameTable* g_nameTable = nullptr;



//----------------------------------------------------------------------------------------------------------------------
void NameTable::Startup()
{
	Name invalidName(Name::s_invalidNameString);
}



//----------------------------------------------------------------------------------------------------------------------
void NameTable::Shutdown()
{
	m_nameTable.clear();
	m_lookupTable.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void NameTable::AppendDebugString(std::string& out) const
{
	for (auto& [name, id] : m_lookupTable)
	{
		out += StringUtils::StringF("%s -> %u\n", name.c_str(), id);
	}
	out += StringUtils::StringF("Total Names: %zu\n", m_lookupTable.size());
}
