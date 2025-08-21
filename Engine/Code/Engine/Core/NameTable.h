// Bradley Christensen - 2025
#pragma once
#include "Engine/Core/StringUtils.h"
#include <unordered_map>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
// THE Name Table
//
extern class NameTable* g_nameTable;




//----------------------------------------------------------------------------------------------------------------------
// NameTable
//
// String set, case insensitive.
//
class NameTable
{
public:

	void Startup();
	void Shutdown();

protected:

	std::unordered_map<std::string, size_t, StringUtils::CaseInsensitiveStringHash, StringUtils::CaseInsensitiveStringEquals> m_lookupTable;
	std::vector<std::string> m_nameTable;

	friend struct Name;
};