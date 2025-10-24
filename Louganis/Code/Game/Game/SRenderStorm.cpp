// Bradley Christensen - 2022-2025
#include "SRenderStorm.h"
#include "SCStorm.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderStorm::Startup()
{
	AddReadDependencies<SCStorm>();
	AddWriteDependencies<Renderer>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderStorm::Run(SystemContext const& context)
{
	SCStorm const& scStorm = g_ecs->GetSingleton<SCStorm>();

	scStorm.
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderStorm::Shutdown()
{
}
