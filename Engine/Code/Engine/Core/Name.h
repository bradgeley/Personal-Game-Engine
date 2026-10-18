// Bradley Christensen - 2022-2026
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"
#include <string>
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
// Name
//
// Index into the name string array
//
struct Name
{
public:

	Name();
	Name(std::string const& string);
	Name(const char* string);
	Name(Name const& other);

	bool IsValid() const;
	uint32_t GetNameIndex() const;
	std::string const& ToString() const;
	const char* ToCStr() const;

	bool operator==(Name const& other) const;
	bool operator!=(Name const& other) const;

protected:

	Name(uint32_t index);

public:

	static Name Invalid;
	static std::string s_invalidNameString;
	static uint32_t s_invalidNameIndex;

protected:

	uint32_t m_nameIndex = s_invalidNameIndex;
};



//----------------------------------------------------------------------------------------------------------------------
namespace std
{
	template <>
	struct hash<Name>
	{
		size_t operator()(Name const& name) const noexcept
		{
			return static_cast<size_t>(std::hash<uint32_t>()(name.GetNameIndex()));
		}
	};
}