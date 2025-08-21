// Bradley Christensen - 2025
#pragma once
#include "Engine/Core/StringUtils.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
// Name
//
// Index into the name string array
//
struct Name
{
public:

	Name(std::string const& string = "Invalid Name");
	Name(const char* string = "Invalid Name");
	Name(Name const& other);

	bool IsValid() const;
	size_t GetNameIndex() const;
	std::string const& ToString() const;
	const char* ToCStr() const;

	bool operator==(Name const& other) const;

protected:

	Name(size_t index);

public:

	static Name s_invalidName;
	static uint32_t s_invalidNameIndex;

protected:

	size_t m_nameIndex = s_invalidNameIndex;
};



//----------------------------------------------------------------------------------------------------------------------
namespace std
{
	template <>
	struct hash<Name>
	{
		size_t operator()(Name const& name) const noexcept
		{
			return std::hash<size_t>()(name.GetNameIndex());
		}
	};
}