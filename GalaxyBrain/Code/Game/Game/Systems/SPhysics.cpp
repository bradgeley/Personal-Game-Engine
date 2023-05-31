// Bradley Christensen - 2023
#include "SPhysics.h"
#include "Game/Game/Components/CPhysics.h"
#include "Game/Game/Components/CTransform.h"



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Startup()
{
    AddWriteDependencies<CPhysics, CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Run(SystemContext const& context)
{
    auto& physStorage = g_ecs->GetArrayStorage<CPhysics>();
    auto& transforms = g_ecs->GetArrayStorage<CTransform>();
    
    for (auto it = g_ecs->Iterate<CTransform, CPhysics>(context); it.IsValid(); ++it)
    {
        EntityID& ent = it.m_currentIndex;
        auto& phys = physStorage[ent];
        auto& transform = transforms[ent];

        // Acceleration -> Velocity
        phys.m_velocity += phys.m_frameAcceleration * context.m_deltaSeconds;
        phys.m_frameAcceleration = Vec2::ZeroVector;

        // Velocity -> Position
        transform.m_pos += phys.m_velocity * context.m_deltaSeconds;

        // Todo: Orientation
    }
}
