// Bradley Christensen - 2022-2025
#include "SCamera.h"
#include "SCCamera.h"
#include "CPlayerController.h"
#include "CRender.h"
#include "Engine/Window/Window.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Startup()
{
    AddWriteDependencies<SCCamera>();
    AddReadDependencies<CRender, CPlayerController>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Run(SystemContext const& context)
{
	SCCamera& camera = g_ecs->GetSingleton<SCCamera>();
	auto& renderStorage = g_ecs->GetArrayStorage<CRender>();

    Vec2 cameraDims = Vec2(g_window->GetAspect() * camera.m_baseOrthoHeight, camera.m_baseOrthoHeight);
    Vec2 cameraMins = -cameraDims * 0.5f * camera.m_zoomAmount;
    Vec2 cameraMaxs =  cameraDims * 0.5f * camera.m_zoomAmount;
    AABB2 cameraBounds = AABB2(cameraMins, cameraMaxs);
    camera.m_camera.SetOrthoBounds2D(cameraBounds);

    for (auto it = g_ecs->Iterate<CRender, CPlayerController>(context); it.IsValid();)
    {
		camera.m_attachedEntity = it.m_currentIndex;
        break;
	}

	bool attachedEntityExists = g_ecs->DoesEntityExist(camera.m_attachedEntity);
    if (attachedEntityExists)
    {
        CRender& render = *renderStorage.Get(camera.m_attachedEntity);
        camera.m_camera.SetPosition2D(render.GetRenderPosition());
	}
    else
    {
		camera.m_attachedEntity = ENTITY_ID_INVALID;
    }

    g_renderer->BeginCameraAndWindow(&camera.m_camera, g_window);
}
