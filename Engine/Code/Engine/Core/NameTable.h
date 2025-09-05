// Bradley Christensen - 2022-2025
#pragma once
#include "StringUtils.h"
#include <unordered_map>
#include <queue>
#include <mutex>



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

	std::mutex m_lookupTableMutex;
	std::unordered_map<std::string, size_t, StringUtils::CaseInsensitiveStringHash, StringUtils::CaseInsensitiveStringEquals> m_lookupTable;
	std::deque<std::string> m_nameTable; // Deque to avoid reallocations that would invalidate pointers to strings in the table, for thread safety

	friend struct Name;
};