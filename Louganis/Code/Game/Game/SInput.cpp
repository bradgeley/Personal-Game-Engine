// Bradley Christensen - 2022-2025
#include "SInput.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Math/MathUtils.h"
#include "CAbility.h"
#include "CCamera.h"
#include "CPlayerController.h"
#include "CMovement.h"
#include "CTransform.h"



//----------------------------------------------------------------------------------------------------------------------
void SInput::Startup()
{
    AddWriteDependencies<CCamera, CMovement, CAbility>();
    AddReadDependencies<InputSystem, CPlayerController, CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInput::Run(SystemContext const& context)
{
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& abilityStorage = g_ecs->GetArrayStorage<CAbility>();
    auto& cameraStorage = g_ecs->GetMapStorage<CCamera>();

    for (auto it = g_ecs->Iterate<CMovement, CCamera, CPlayerController, CAbility>(context); it.IsValid(); ++it)
    {
        CMovement& move = moveStorage[it];
        CCamera& camera = cameraStorage[it];
        CTransform const& transform = transformStorage[it];
        CAbility& ability = abilityStorage[it];

        if (g_input->WasMouseButtonJustPressed(0))
        {
			ability.m_isCastingAbility = true;
        }

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

        if (g_input->IsKeyDown(KeyCode::Shift) && !ability.m_isCastingAbility)
        {
            move.m_isSprinting = true;
        }
        else
        {
            move.m_isSprinting = false;
        }

        int wheelChange = g_input->GetMouseWheelChange();
        if (wheelChange != 0)
        {
            float zoomMulti = 1.f + static_cast<float>(-1 * wheelChange) * camera.m_zoomMultiplier;
            camera.m_zoomAmount *= zoomMulti;
            camera.m_zoomAmount = MathUtils::Clamp(camera.m_zoomAmount, camera.m_minZoom, camera.m_maxZoom);
        }

        if (g_input->IsKeyDown(KeyCode::Ctrl) && g_input->WasKeyJustPressed('T'))
        {
            Vec2 relMousePos = g_input->GetMouseClientRelativePosition();
            Vec2 worldMousePos = camera.m_camera.ScreenToWorldOrtho(relMousePos);
            move.m_frameMovement = worldMousePos - transform.m_pos;
            move.m_isTeleporting = true;
		}
        else
        {
            move.m_isTeleporting = false;
        }
    }
}
