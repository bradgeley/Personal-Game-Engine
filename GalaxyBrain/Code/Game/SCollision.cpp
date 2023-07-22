// Bradley Christensen - 2023
#include "SCollision.h"
#include "Game/CCollision.h"
#include "Game/CTransform.h"
#include "Game/CPhysics.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Startup()
{
    AddWriteDependencies<CTransform, CPhysics, CCollision>();

    m_systemSplittingNumJobs = 1;
}



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& physStorage  = g_ecs->GetArrayStorage<CPhysics>();
    auto& collStorage  = g_ecs->GetArrayStorage<CCollision>();

    auto itA = g_ecs->Iterate<CTransform, CCollision, CPhysics>(context);
    auto itB = itA;

    for (/*init above*/; itA.IsValid(); ++itA)
    {
        CTransform& transA = transStorage[itA.m_currentIndex];
        CCollision& collA = *collStorage.Get(itA.m_currentIndex);
        CPhysics& physA = *physStorage.Get(itA.m_currentIndex); // optional

        Vec2& posA = transA.m_pos;
        float& radiusA = collA.m_radius;

        // Start iterator B at one entity after iterator A (without copying a bunch of data every iteration by calling itB = itA;
        itB.m_currentIndex = itA.m_currentIndex;
        ++itB;

        for (/*init above*/; itB.IsValid(); ++itB)
        {
            CTransform& transB = transStorage[itB.m_currentIndex];
            CCollision& collB = *collStorage.Get(itB.m_currentIndex);

            Vec2& posB = transB.m_pos;
            float& radiusB = collB.m_radius;

            if (transB.m_attachedToEntity == itA.m_currentIndex || transA.m_attachedToEntity == itB.m_currentIndex)
            {
                // Ignore collision with attached actors, but detach them if they are no longer overlapping
                if (!DoDiscsOverlap2D(posB, radiusB, posA, radiusA))
                {
                    DetachEntities(itA.m_currentIndex, itB.m_currentIndex);
                }
                continue;
            }

            CPhysics& physB = *physStorage.Get(itB.m_currentIndex); // optional

            bool ACanPushB = (collA.m_type == CollisionType::Static && collB.m_type == CollisionType::Mobile);
            bool BCanPushA = (collA.m_type == CollisionType::Mobile && collB.m_type == CollisionType::Static);
            bool PushEachOther = (collA.m_type == CollisionType::Mobile && collB.m_type == CollisionType::Mobile);

            bool didPush = false;
            if (ACanPushB)
            {
                didPush = PushDiscOutOfDisc2D(posB, radiusB, posA, radiusA);
            }
            else if (BCanPushA)
            {
                didPush = PushDiscOutOfDisc2D(posA, radiusA, posB, radiusB);
            }
            else if (PushEachOther)
            {
                didPush = BounceDiscsOffEachOther2D(posA, radiusA, physA.m_velocity, physA.m_mass, posB, radiusB, physB.m_velocity, physB.m_mass, 0.f);
            }

            if (didPush)
            {
                bool ACanAttachToB = (collA.m_attachType == AttachmentType::CanAttach && collB.m_attachType == AttachmentType::CanHaveAttachedEntities);
                if (ACanAttachToB)
                {
                    transA.m_attachedToEntity = itB.m_currentIndex;
                    Vec2 toCenter = (transA.m_pos - transB.m_pos);
                    float relativeAngle = toCenter.GetAngleDegrees() - transB.m_orientation;
                    transA.m_polarCoords = Vec2(relativeAngle, toCenter.GetLength() * 0.9999f); // Move in just a bit to prevent skating along the edge
                    physA.m_velocity = Vec2::ZeroVector;
                }
                bool BCanAttachToA = (collA.m_attachType == AttachmentType::CanHaveAttachedEntities && collB.m_attachType == AttachmentType::CanAttach);
                if (BCanAttachToA)
                {
                    transB.m_attachedToEntity = itA.m_currentIndex;
                    Vec2 toCenter = (transB.m_pos - transA.m_pos);
                    float relativeAngle = toCenter.GetAngleDegrees() - transA.m_orientation;
                    transB.m_polarCoords = Vec2(relativeAngle, toCenter.GetLength());
                    physB.m_velocity = Vec2::ZeroVector;
                }
            }
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SCollision::DetachEntities(EntityID a, EntityID b) const
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& physStorage = g_ecs->GetArrayStorage<CPhysics>();

    CTransform& transA = transStorage[a];
    CTransform& transB = transStorage[b];

    CPhysics& physA = physStorage[a];
    CPhysics& physB = physStorage[b];

    if (transA.m_attachedToEntity == b)
    {
        transA.m_attachedToEntity = ENTITY_ID_INVALID;

        Vec2 toCenter = (transA.m_pos - transB.m_pos);
        float radius = toCenter.GetLength();
        toCenter /= radius;
        float speedAtRadius = DegreesToRadians(physB.m_angularVelocity) * radius;
        physA.m_velocity = physB.m_velocity + speedAtRadius * toCenter.GetRotated90();
    }

    if (transB.m_attachedToEntity == a)
    {
        transB.m_attachedToEntity = ENTITY_ID_INVALID;

        Vec2 toCenter = (transB.m_pos - transA.m_pos);
        float radius = toCenter.GetLength();
        toCenter /= radius;
        float speedAtRadius = DegreesToRadians(physA.m_angularVelocity) * radius;
        physB.m_velocity = physA.m_velocity + speedAtRadius * toCenter.GetRotated90();
    }
}
