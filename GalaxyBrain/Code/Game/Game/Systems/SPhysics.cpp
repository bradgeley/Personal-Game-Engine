// Bradley Christensen - 2023
#include "SPhysics.h"
#include "Game/Game/Components/CPhysics.h"
#include "Game/Game/Components/CTransform.h"
#include "Game/Game/Singletons/SCDebug.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Startup()
{
    AddWriteDependencies<CPhysics, CTransform>();
    AddWriteDependencies<SCDebug>();
}



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Run(SystemContext const& context)
{
    auto& physStorage = g_ecs->GetArrayStorage<CPhysics>();
    auto& transforms = g_ecs->GetArrayStorage<CTransform>();
    auto& scDebug = *g_ecs->GetComponent<SCDebug>();
    
    for (auto it = g_ecs->Iterate<CTransform, CPhysics>(context); it.IsValid(); ++it)
    {
        EntityID& ent = it.m_currentIndex;
        auto& phys = physStorage[ent];
        auto& transform = transforms[ent];

        // Acceleration -> Velocity
        phys.m_velocity += phys.m_frameAcceleration * context.m_deltaSeconds;

        // Velocity -> Position
        transform.m_pos += phys.m_velocity * context.m_deltaSeconds;

        // Todo: Orientation

        if (scDebug.m_physicsDebugDraw)
        {
            AddVertsForLine2D(scDebug.m_debugDrawVerts.GetMutableVerts(), transform.m_pos, transform.m_pos + phys.m_velocity, 1.f, Rgba8::Yellow);
            AddVertsForLine2D(scDebug.m_debugDrawVerts.GetMutableVerts(), transform.m_pos, transform.m_pos + phys.m_frameAcceleration, 1.f, Rgba8::Orange);
        }

        // Reset for next frame
        phys.m_frameAcceleration = Vec2::ZeroVector;
    }
}
