// Bradley Christensen - 2023
#include "SInput.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Math/MathUtils.h"
#include "CMovement.h"
#include "CCamera.h"
#include "CPlayerController.h"



//----------------------------------------------------------------------------------------------------------------------
void SInput::Startup()
{
    AddWriteDependencies<CCamera, CMovement>();
    AddReadDependencies<InputSystem, CPlayerController>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInput::Run(SystemContext const& context)
{
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    for (auto it = g_ecs->Iterate<CMovement, CPlayerController>(context); it.IsValid(); ++it)
    {
        EntityID& ent = it.m_currentIndex;
        auto& move = moveStorage[ent];

        move.m_frameMoveDir = Vec2::ZeroVector;
        if (g_input->IsKeyDown('W'))
        {
            move.m_frameMoveDir += Vec2(0.f, 1.f);
        }
        if (g_input->IsKeyDown('A'))
        {
            move.m_frameMoveDir += Vec2(-1.f, 0.f);
        }
        if (g_input->IsKeyDown('S'))
        {
            move.m_frameMoveDir += Vec2(0.f, -1.f);
        }
        if (g_input->IsKeyDown('D'))
        {
            move.m_frameMoveDir += Vec2(1.f, 0.f);
        }
        move.m_frameMoveDir.Normalize();

        if (g_input->IsKeyDown(KeyCode::Shift))
        {
            move.m_isSprinting = true;
        }
        else
        {
            move.m_isSprinting = false;
        }
    }

    auto& cameraStorage = g_ecs->GetMapStorage<CCamera>();
    for (auto it = g_ecs->Iterate<CCamera, CPlayerController>(context); it.IsValid(); ++it)
    {
        EntityID& ent = it.m_currentIndex;
        auto& camera = cameraStorage[ent];

        int wheelChange = g_input->GetMouseWheelChange();
        if (wheelChange != 0)
        {
            float zoomMulti = 1.f + static_cast<float>(-1 * wheelChange) * camera.m_zoomMultiplier;
            camera.m_zoomAmount *= zoomMulti;
            camera.m_zoomAmount = MathUtils::ClampF(camera.m_zoomAmount, camera.m_minZoom, camera.m_maxZoom);
        }
    }
}
