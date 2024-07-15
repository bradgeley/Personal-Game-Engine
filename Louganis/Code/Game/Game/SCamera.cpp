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

        camera.m_camera.SetPosition2D(transform.m_pos);
        Vec2 cameraDims = Vec2(g_window->GetAspect() * camera.m_tileHeight, camera.m_tileHeight);
        camera.m_camera.SetOrthoDims2D(cameraDims);
    }
}
