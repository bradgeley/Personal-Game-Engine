// Bradley Christensen - 2023
#include "SGravity.h"
#include "Game/CPhysics.h"
#include "Game/CTransform.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr float GRAVITATIONAL_CONSTANT = 10.f;



//----------------------------------------------------------------------------------------------------------------------
void SGravity::Startup()
{
    AddWriteDependencies<CPhysics>();
    AddReadDependencies<CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SGravity::Run(SystemContext const& context)
{
    auto& physStorage = g_ecs->GetArrayStorage<CPhysics>();
    auto& transforms = g_ecs->GetArrayStorage<CTransform>();
    
    for (auto itA = g_ecs->Iterate<CTransform, CPhysics>(context); itA.IsValid(); ++itA)
    {
        EntityID& entA = itA.m_currentIndex;
        auto& physA = physStorage[entA];
        if (physA.m_mass == 0.f)
        {
            continue;
        }
        
        auto& transA = transforms[entA];

        for (auto itB = g_ecs->Iterate<CTransform, CPhysics>(context); itB.IsValid(); ++itB)
        {
            EntityID& entB = itB.m_currentIndex;
            if (entA == entB)
            {
                continue;
            }
            
            auto& transB = transforms[entB];
            Vec2 bToA = transA.m_pos - transB.m_pos;
            float distanceSquared = bToA.GetLengthSquared();
            if (distanceSquared <= 0.001f)
            {
                continue;
            }

            // Gravtity equations: 
            // F = ma
            // F = G(m1 * m2) / d^2
            // a = (G * m2) / r^2
            
            auto& physB = physStorage[entB];
            physB.m_frameAcceleration += (bToA * GRAVITATIONAL_CONSTANT * physA.m_mass) / distanceSquared;
        }
    }
}
