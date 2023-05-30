// Bradley Christensen - 2023
#include "SMovement.h"
#include "Game/Game/Components/CMovement.h"
#include "Game/Game/Components/CPhysics.h"



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Startup()
{
    AddWriteDependencies<CMovement, CPhysics>();
}



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Run(SystemContext const& context)
{
    auto& physStorage = m_admin->GetArrayStorage<CPhysics>();
    auto& moveStorage = m_admin->GetArrayStorage<CMovement>();
    for (auto it = Iterate<CMovement, CPhysics>(context); it.IsValid(); ++it)
    {
        EntityID& ent = it.m_currentIndex;
        auto& phys = physStorage[ent];
        auto& move = moveStorage[ent];

        phys.m_frameAcceleration = move.m_frameMoveDir * move.m_movementSpeed * 10.f;
    }
}
