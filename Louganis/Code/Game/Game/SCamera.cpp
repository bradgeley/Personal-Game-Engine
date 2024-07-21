// Bradley Christensen - 2023
#include "SCamera.h"
#include "CCamera.h"
#include "CTransform.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Startup()
{
    AddWriteDependencies<CCamera>();
    AddReadDependencies<CTransform, InputSystem>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Run(SystemContext const& context)
{
    for (auto it = g_ecs->Iterate<CTransform, CCamera>(context); it.IsValid(); ++it)
    {
        CTransform& transform = *g_ecs->GetComponent<CTransform>(it.m_currentIndex);
        CCamera& camera = *g_ecs->GetComponent<CCamera>(it.m_currentIndex);

        Vec2 cameraDims = Vec2(g_window->GetAspect() * camera.m_baseOrthoHeight, camera.m_baseOrthoHeight);
        Vec2 cameraMins = -cameraDims * 0.5f * camera.m_zoomAmount;
        Vec2 cameraMaxs =  cameraDims * 0.5f * camera.m_zoomAmount;
        AABB2 cameraBounds = AABB2(cameraMins, cameraMaxs);
        camera.m_camera.SetOrthoBounds2D(cameraBounds);
        camera.m_camera.SetPosition2D(transform.m_pos);
    }
}
