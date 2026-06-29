// Bradley Christensen - 2022-2026
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
	bool Resolve(uint8_t tags) const;

	uint8_t m_tagsToQuery	= 0;
	TagQueryOp m_queryOp	= TagQueryOp::DoesNotHaveAny;
};