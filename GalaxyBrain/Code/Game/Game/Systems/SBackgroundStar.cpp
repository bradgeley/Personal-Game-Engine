// Bradley Christensen - 2023
#include "SBackgroundStar.h"
#include "Game/Game/Singletons/SCUniverse.h"
#include "Engine/ECS/AdminSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundStar::Startup()
{
	AddWriteDependencies<SCUniverse>();

	SCUniverse& univ = *g_ecs->GetComponent<SCUniverse>();
}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundStar::Run(SystemContext const& context)
{

}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundStar::Shutdown()
{

}
