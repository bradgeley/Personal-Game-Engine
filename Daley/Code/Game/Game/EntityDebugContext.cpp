// Bradley Christensen - 2022-2026
#include "EntityDebugContext.h"
#include "CTime.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
EntityDebugContext::EntityDebugContext(EntityID eid, SystemContext const& context)
{
	CTime const* time = context.GetComponentConst<CTime>(eid);
	if (time)
	{
		m_isSlowed = time->IsSlowed();
		m_isHasted = time->IsHasted();
		m_timeDilation = time->m_clock.GetTimeDilationF();
	}
}
