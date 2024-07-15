// Bradley Christensen - 2023
#include "SMovement.h"
#include "CMovement.h"
#include "CTransform.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Startup()
{
    AddWriteDependencies<CMovement, CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Run(SystemContext const& context)
{
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();

    for (auto it = g_ecs->Iterate<CMovement, CTransform>(context); it.IsValid(); ++it)
    {
        EntityID& ent = it.m_currentIndex;
        CMovement& move = moveStorage[ent];
        CTransform& transform = transStorage[ent];

        transform.m_pos += move.m_frameMoveDir * move.m_movementSpeed;
    }
}
