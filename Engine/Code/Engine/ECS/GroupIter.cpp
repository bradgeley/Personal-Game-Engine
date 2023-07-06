// Bradley Christensen - 2023
#include "GroupIter.h"
#include "AdminSystem.h"
#include "SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
GroupIter::GroupIter(SystemContext const& context) : m_context(&context)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool GroupIter::IsValid() const
{
	return m_currentIndex <= m_context->m_endEntityID;
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
	while (m_currentIndex <= m_context->m_endEntityID);
}



//----------------------------------------------------------------------------------------------------------------------
void GroupIter::operator++()
{
	Next();
}
