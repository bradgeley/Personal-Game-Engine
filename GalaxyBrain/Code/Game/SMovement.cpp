// Bradley Christensen - 2023
#include "SMovement.h"
#include "Game/CMovement.h"
#include "Game/CPhysics.h"
#include "Game/CTransform.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Startup()
{
    AddWriteDependencies<CTransform, CMovement, CPhysics>();
}



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    auto& physStorage = g_ecs->GetArrayStorage<CPhysics>();

    for (auto it = g_ecs->Iterate<CTransform, CMovement, CPhysics>(context); it.IsValid(); ++it)
    {
        EntityID& ent = it.m_currentIndex;
        auto& transform = transStorage[ent];
        auto& move = moveStorage[ent];
        auto& phys = physStorage[ent];

        if (transform.m_attachedToEntity != ENTITY_ID_INVALID)
        {
            move.m_mode = MovementMode::PolarCoords;
        }
        else
        {
            move.m_mode = MovementMode::Acceleration;
        }

        if (move.m_frameMoveDir == Vec2::ZeroVector)
        {
            phys.m_polarVelocity = Vec2::ZeroVector;
            continue;
        }

        switch (move.m_mode)
        {
            case MovementMode::Acceleration:
            {
                phys.m_frameAcceleration += move.m_frameMoveDir * move.m_movementSpeed;
                break;
            }
            case MovementMode::PolarCoords:
            {
                EntityID attachedTo = transform.m_attachedToEntity;
                auto& attachedToTransform = transStorage[attachedTo];

                float radius = (attachedToTransform.m_pos - transform.m_pos).GetLength();
                float arcLengthPerSecondMoveSpeed = move.m_movementSpeed;
                float degreesPerSecondMoveSpeed = RadiansToDegrees(arcLengthPerSecondMoveSpeed / radius);
                phys.m_polarVelocity = move.m_frameMoveDir * Vec2(degreesPerSecondMoveSpeed, move.m_movementSpeed);
                break;
            }
        }
    }
}
