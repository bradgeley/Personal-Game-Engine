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
        camera.m_zoom += -1.f * mouseWheelChange * 0.1f;
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

        camera.m_camera.SetOrthoCenter2D(camPos);
    }
}
