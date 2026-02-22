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
