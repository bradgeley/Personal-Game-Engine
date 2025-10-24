// Bradley Christensen - 2022-2025
#include "SStorm.h"
#include "SCStorm.h"



//----------------------------------------------------------------------------------------------------------------------
void SStorm::Startup()
{
	AddWriteDependencies<SCStorm>();
}



//----------------------------------------------------------------------------------------------------------------------
void SStorm::Run(SystemContext const& context)
{
	SCStorm& scStorm = g_ecs->GetSingleton<SCStorm>();

	scStorm.m_stormRadius += scStorm.m_stormRadiusGrowthRate * context.m_deltaSeconds;
}