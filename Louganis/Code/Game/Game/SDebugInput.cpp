// Bradley Christensen - 2022-2025
#include "SDebugInput.h"
#include "Engine/Input/InputSystem.h"
#include "SCCamera.h"
#include "SCWorld.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Startup()
{
    AddWriteDependencies<InputSystem, SCCamera>();
	AddReadDependencies<SCWorld>();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Run(SystemContext const&)
{
	SCCamera& camera = g_ecs->GetSingleton<SCCamera>();

	if (g_input->WasKeyJustPressed('Z'))
	{
		if (camera.m_zoomAmount == camera.m_maxZoom)
		{
			camera.m_zoomAmount = camera.m_minZoom;
		}
		else
		{
			camera.m_zoomAmount = camera.m_maxZoom;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Shutdown()
{

}
