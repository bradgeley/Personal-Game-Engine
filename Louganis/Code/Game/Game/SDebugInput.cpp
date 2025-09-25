// Bradley Christensen - 2022-2025
#include "SDebugInput.h"
#include "Engine/Input/InputSystem.h"
#include "CCamera.h"
#include "SCWorld.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Startup()
{
    AddWriteDependencies<InputSystem, CCamera>();
	AddReadDependencies<SCWorld>();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Run(SystemContext const& context)
{
	auto& cameraStorage = g_ecs->GetMapStorage<CCamera>();

	if (g_input->WasKeyJustPressed('Z'))
	{
		for (auto it = g_ecs->Iterate<CCamera>(context); it.IsValid(); ++it)
		{
			CCamera* camera = cameraStorage.Get(it);
			if (camera->m_isActive)
			{
				if (camera->m_zoomAmount == camera->m_maxZoom)
				{
					camera->m_zoomAmount = camera->m_minZoom;
				}
				else
				{
					camera->m_zoomAmount = camera->m_maxZoom;
				}
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Shutdown()
{

}
