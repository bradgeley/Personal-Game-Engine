// Bradley Christensen - 2023
#include "SCamera.h"
#include "Game/CCamera.h"
#include "Game/CTransform.h"
#include "Game/CPhysics.h"
#include "Engine/Input/InputSystem.h"



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

        camera.m_camera.SetPosition(Vec3(transform.m_pos));
        camera.m_camera.SetRotation2D(transform.m_orientation);

        // Zoom
        int mouseWheelChange = g_input->GetMouseWheelChange();
        while (mouseWheelChange > 0)
        {
            camera.m_zoom *= 0.75f;
            --mouseWheelChange;

            if (camera.m_zoom < camera.m_minZoom)
            {
                camera.m_zoom = camera.m_minZoom;
                break;
            }
        }

        while (mouseWheelChange < 0)
        {
            camera.m_zoom *= 1.25f;
            ++mouseWheelChange;

            if (camera.m_zoom > camera.m_maxZoom)
            {
                camera.m_zoom = camera.m_maxZoom;
                break;
            }
        }

        camera.m_camera.SetOrthoDims2D(camera.m_baseDims * camera.m_zoom);
    }
}
