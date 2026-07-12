// Bradley Christensen - 2022-2026
#include "TagQuery.h"



//----------------------------------------------------------------------------------------------------------------------
bool TagQuery::Resolve(uint8_t tags) const
{
	bool hasAnyResult			= (m_hasAnyTags			!= 0) ? (tags & m_hasAnyTags) != 0								: true;
	bool doesNotHaveAnyResult	= (m_doesNotHaveAnyTags	!= 0) ? (tags & m_doesNotHaveAnyTags) == 0						: true;
	bool hasAllResult			= (m_hasAllTags			!= 0) ? (tags & m_hasAllTags) == m_hasAllTags					: true;
	bool doesNotHaveAllResult	= (m_doesNotHaveAllTags	!= 0) ? (tags & m_doesNotHaveAllTags) != m_doesNotHaveAllTags	: true;

	if (m_queryOp == TagQueryOp::Any)
	{
		return hasAnyResult || doesNotHaveAnyResult || hasAllResult || doesNotHaveAllResult;
	}
	else // All
	{
		return hasAnyResult && doesNotHaveAnyResult && hasAllResult && doesNotHaveAllResult;
	}
}