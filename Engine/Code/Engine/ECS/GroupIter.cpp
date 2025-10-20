// Bradley Christensen - 2022-2025
#include "GroupIter.h"
#include "AdminSystem.h"
#include "SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
GroupIter::GroupIter(SystemContext const& context)
{
	m_currentIndex = context.m_startEntityID;
	m_endIndex = context.m_endEntityID;
}



//----------------------------------------------------------------------------------------------------------------------
bool GroupIter::IsValid() const
{
	return m_currentIndex <= m_endIndex;
}



//----------------------------------------------------------------------------------------------------------------------
void GroupIter::Next()
{
	do
	{
		++m_currentIndex;
		if ((g_ecs->m_entityComposition[m_currentIndex] & m_groupMask) == m_groupMask)
		{
			return;
		}
	}
	while (m_currentIndex <= m_endIndex);
}



//----------------------------------------------------------------------------------------------------------------------
void GroupIter::operator++()
{
	Next();
}
