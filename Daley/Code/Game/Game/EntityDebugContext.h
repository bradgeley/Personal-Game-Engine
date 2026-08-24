// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/EntityID.h"
#include <string>



struct SystemContext;



//----------------------------------------------------------------------------------------------------------------------
// This class contains debug information about an entity that might affect other parts of the entity, like movement speed
// being affected by time dilation, so that those systems can display the correct information.
//
struct EntityDebugContext
{
	EntityDebugContext(EntityID eid, SystemContext const& context);

	std::string m_debugString;
	float m_timeDilation = 1.f;
	bool m_isHasted = false;
	bool m_isSlowed = false;
};