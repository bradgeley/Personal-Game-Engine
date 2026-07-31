// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <string>
#include <array>



//----------------------------------------------------------------------------------------------------------------------
constexpr int MAX_TAGS = 8;



//----------------------------------------------------------------------------------------------------------------------
struct CTags
{
public:

    CTags() = default;
    CTags(void const* xmlElement);

	bool AddTag(Name const& tag);
	bool RemoveTag(Name const& tag);
	bool HasTag(Name const& tag) const;
	int FindTag(Name const& tag) const; // returns the index of the tag

	void AppendDebugString(std::string& out) const;

private:
    
    std::array<Name, MAX_TAGS> m_tags = { Name::Invalid };
};
