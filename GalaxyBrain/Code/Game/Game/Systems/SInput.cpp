// Bradley Christensen - 2023
#include "SInput.h"
#include "Engine/Input/InputSystem.h"
#include "Game/Game/Components/CMovement.h"



//----------------------------------------------------------------------------------------------------------------------
void SInput::Startup()
{
    AddWriteDependencies<CMovement>();
    AddReadDependencies<InputSystem>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInput::Run(SystemContext const& context)
{
    auto& moveStorage = m_admin->GetArrayStorage<CMovement>();
    for (auto it = Iterate<CMovement>(context); it.IsValid(); ++it)
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
    }
}
