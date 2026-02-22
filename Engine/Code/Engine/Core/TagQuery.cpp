// Bradley Christensen - 2022-2026
#include "TagQuery.h"



//----------------------------------------------------------------------------------------------------------------------
bool TagQuery::Resolve(uint8_t tags) const
{
    switch (m_queryOp)
    {
        case TagQueryOp::HasAny:
        {
            return (tags & m_tagsToQuery) != 0;
        }
        case TagQueryOp::HasAll:
        {
            return (tags & m_tagsToQuery) == m_tagsToQuery;
        }
        case TagQueryOp::DoesNotHaveAny:
        {
            return (tags & m_tagsToQuery) != m_tagsToQuery;
        }
        case TagQueryOp::DoesNotHaveAll:
        {
            return (tags & m_tagsToQuery) == 0;
        }
    }
    return false;
}