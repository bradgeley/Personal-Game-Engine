// Bradley Christensen - 2022-2026
#pragma once
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class TagQueryOp
{
	Any, // Any of the 4 categories must be true for the query to return true
	All, // All of the 4 categories must be true for the query to return true
};



//----------------------------------------------------------------------------------------------------------------------
struct TagQuery
{
	bool Resolve(uint8_t tags) const;

	TagQueryOp m_queryOp = TagQueryOp::All;

	uint8_t m_hasAllTags = 0;
	uint8_t m_doesNotHaveAllTags = 0;
	uint8_t m_hasAnyTags = 0;
	uint8_t m_doesNotHaveAnyTags = 0;
};