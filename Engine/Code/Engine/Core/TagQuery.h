// Bradley Christensen - 2022-2026
#pragma once
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
struct TagQuery
{
	bool Resolve(uint8_t tags) const;

	uint8_t m_hasAllTags = 0;
	uint8_t m_doesNotHaveAllTags = 0;
	uint8_t m_hasAnyTags = 0;
	uint8_t m_doesNotHaveAnyTags = 0;
};