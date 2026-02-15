// Bradley Christensen - 2022-2025
#include "TagQuery.h"



//----------------------------------------------------------------------------------------------------------------------
bool TagQuery::Resolve(uint8_t tileTags) const
{
    switch (m_queryOp)
    {
        case TagQueryOp::HasAny:
        {
            return (tileTags & m_tagsToQuery) != 0;
        }
        case TagQueryOp::HasAll:
        {
            return (tileTags & m_tagsToQuery) == m_tagsToQuery;
        }
        case TagQueryOp::DoesNotHaveAny:
        {
            return (tileTags & m_tagsToQuery) != m_tagsToQuery;
        }
        case TagQueryOp::DoesNotHaveAll:
        {
            return (tileTags & m_tagsToQuery) == 0;
        }
    }
    return false;
}