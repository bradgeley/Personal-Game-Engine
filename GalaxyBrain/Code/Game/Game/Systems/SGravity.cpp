// Bradley Christensen - 2023
#include "SGravity.h"
#include "Game/Game/Components/CPhysics.h"
#include "Game/Game/Components/CTransform.h"



//----------------------------------------------------------------------------------------------------------------------
void SGravity::Startup()
{
    AddWriteDependencies<CPhysics>();
    AddReadDependencies<CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SGravity::Run(SystemContext const& context)
{
    auto& physStorage = m_admin->GetArrayStorage<CPhysics>();
    auto& transforms = m_admin->GetArrayStorage<CTransform>();
    
    for (auto itA = Iterate<CTransform, CPhysics>(context); itA.IsValid(); ++itA)
    {
        EntityID& entA = itA.m_currentIndex;
        auto& physA = physStorage[entA];
        if (physA.m_gravityStrength == 0.f)
        {
            continue;
        }
        
        auto& transA = transforms[entA];

        for (auto itB = Iterate<CTransform, CPhysics>(context); itB.IsValid(); ++itB)
        {
            EntityID& entB = itB.m_currentIndex;
            if (entA == entB)
            {
                continue;
            }
            
            auto& transB = transforms[entB];
            Vec2 bToA = transA.m_pos - transB.m_pos;
            float distanceSquared = bToA.GetLengthSquared();
            if (distanceSquared == 0.f)
            {
                continue;
            }
            
            auto& physB = physStorage[entB];
            physB.m_frameAcceleration += (bToA * physA.m_gravityStrength) / distanceSquared;
        }
    }
}
