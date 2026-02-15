// Bradley Christensen - 2022-2025
#pragma once
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class TagQueryOp
{
	HasAll,
	DoesNotHaveAll,
	HasAny,
	DoesNotHaveAny,
};



//----------------------------------------------------------------------------------------------------------------------
struct TagQuery
{
	bool Resolve(uint8_t tileTags) const;

	uint8_t m_tagsToQuery;
	TagQueryOp m_queryOp;
};