// Bradley Christensen - 2022-2026
#pragma once
#include "StringUtils.h"
#include <shared_mutex>
#include <unordered_map>
#include <deque>



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

	NameTable();
	static NameTable& Get();

	void Startup();
	void Shutdown();

	void AppendDebugString(std::string& out) const;

protected:

	mutable std::shared_mutex m_lookupTableMutex;
	std::unordered_map<std::string, size_t, StringUtils::CaseInsensitiveStringHash, StringUtils::CaseInsensitiveStringEquals> m_lookupTable;
	std::deque<std::string> m_nameTable;

	friend struct Name;
};