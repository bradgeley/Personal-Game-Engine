// Bradley Christensen - 2023
#include "SPhysics.h"
#include "Game/CPhysics.h"
#include "Game/CTransform.h"
#include "Game/SCDebug.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Math/MathUtils.h"



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

        // Angular Velocity -> Orientation
        transform.m_orientation += phys.m_angularVelocity * context.m_deltaSeconds;

        // Polar Coords (if attached to an actor)
        if (transform.m_attachedToEntity != ENTITY_ID_INVALID)
        {
            transform.m_polarCoords += phys.m_polarVelocity * context.m_deltaSeconds;
            CTransform const& attachedToTransform = transforms[transform.m_attachedToEntity];
            float totalOrientation = transform.m_polarCoords.x + attachedToTransform.m_orientation;
            transform.m_pos = attachedToTransform.m_pos + Vec2::MakeFromPolarCoords(totalOrientation, transform.m_polarCoords.y);
        }

        // Debug Draw
        if (scDebug.m_physicsDebugDraw)
        {
            AddVertsForLine2D(scDebug.m_debugDrawVerts.GetMutableVerts(), transform.m_pos, transform.m_pos + phys.m_velocity, 5.f, Rgba8::Yellow);
            AddVertsForLine2D(scDebug.m_debugDrawVerts.GetMutableVerts(), transform.m_pos, transform.m_pos + phys.m_frameAcceleration, 5.f, Rgba8::Orange);
        }

        // Reset for next frame
        phys.m_frameAcceleration = Vec2::ZeroVector;
    }
}
