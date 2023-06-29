// Bradley Christensen - 2023
#include "SUniverse.h"
#include "Game/Game/Singletons/SCUniverse.h"
#include "Engine/ECS/AdminSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SUniverse::Startup()
{
	AddWriteDependencies<SCUniverse>();

	SCUniverse& univ = *g_ecs->GetComponent<SCUniverse>();
}



//----------------------------------------------------------------------------------------------------------------------
void SUniverse::Run(SystemContext const& context)
{
}



//----------------------------------------------------------------------------------------------------------------------
void SUniverse::Shutdown()
{

}
