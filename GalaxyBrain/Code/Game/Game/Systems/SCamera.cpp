// Bradley Christensen - 2023
#include "SCamera.h"
#include "Game/Game/Components/CCamera.h"
#include "Game/Game/Components/CTransform.h"
#include "Game/Game/Components/CPhysics.h"
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

        // Position
        Vec2 camPos = camera.m_camera.GetOrthoCenter2D();
        Vec2 toGoal = transform.m_pos - camPos;

        if (camera.m_snappiness * context.m_deltaSeconds > 1.f)
        {
            // We would overshoot the goal with these numbers, so just snap to it
            camPos = transform.m_pos;
        }
        else
        {
            camPos += toGoal * camera.m_snappiness * context.m_deltaSeconds;
        }

        // Clamp camera to a shorter distance at lower zoom levels
        Vec2 toCamera = camPos - transform.m_pos;
        float maxDistance = 5.f * (camera.m_zoom);
        toCamera.ClampLength(maxDistance);
        camPos = transform.m_pos + toCamera;

        camera.m_camera.SetOrthoCenter2D(camPos);
    }
}
