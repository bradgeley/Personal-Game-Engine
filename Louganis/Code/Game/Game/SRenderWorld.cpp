// Bradley Christensen - 2023
#include "SRenderWorld.h"
#include "SCWorld.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Startup()
{
    AddWriteDependencies<SCWorld, Renderer>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Run(SystemContext const&)
{
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();

    world.Render();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Shutdown()
{

}