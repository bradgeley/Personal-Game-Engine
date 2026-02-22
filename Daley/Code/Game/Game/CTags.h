// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/TagQuery.h"
#include "Engine/Core/Name.h"
#include <array>



//----------------------------------------------------------------------------------------------------------------------
constexpr int MAX_TAGS = 8;



//----------------------------------------------------------------------------------------------------------------------
struct CTags
{
public:

    CTags() = default;
    CTags(void const* xmlElement);

	void AddTag(Name const& tag);
	void RemoveTag(Name const& tag);
	bool HasTag(Name const& tag) const;
	int FindTag(Name const& tag) const; // returns the index of the tag

private:
    
    std::array<Name, MAX_TAGS> m_tags = { Name::Invalid };
};
