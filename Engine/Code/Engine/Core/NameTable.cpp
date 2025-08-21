// Bradley Christensen - 2025
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
