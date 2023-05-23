// Bradley Christensen - 2023
#include "GroupIter.h"
#include "AdminSystem.h"



//----------------------------------------------------------------------------------------------------------------------
GroupIter::GroupIter(SystemContext const& context) : m_context(context)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool GroupIter::IsValid() const
{
	return m_currentIndex <= m_context.m_endEntityID;
}



//----------------------------------------------------------------------------------------------------------------------
void GroupIter::Next()
{
	do
	{
		++m_currentIndex;
		if ((m_context.m_admin->m_entityComposition[m_currentIndex] & m_groupMask) == m_groupMask)
		{
			return;
		}
	}
	while (m_currentIndex <= m_context.m_endEntityID);
}
