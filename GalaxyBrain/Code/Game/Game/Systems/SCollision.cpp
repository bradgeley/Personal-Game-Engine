// Bradley Christensen - 2023
#include "SCollision.h"
#include "Game/Game/Components/CCollision.h"
#include "Game/Game/Components/CTransform.h"
#include "Game/Game/Components/CPhysics.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Math/GeometryUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Startup()
{
    AddWriteDependencies<CTransform, CPhysics, CCollision>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& physStorage  = g_ecs->GetArrayStorage<CPhysics>();
    auto& collStorage  = g_ecs->GetArrayStorage<CCollision>();

    auto itA = g_ecs->Iterate<CTransform, CCollision>(context);
    auto itB = itA;

    for (/*init above*/; itA.IsValid(); ++itA)
    {
        Vec2& posA = transStorage.Get(itA.m_currentIndex)->m_pos;
        CCollision& collA = *collStorage.Get(itA.m_currentIndex);
        float& radiusA = collA.m_radius;

        // Start iterator B at one entity after iterator A (without copying a bunch of data each iteration by calling itB = itA;
        itB.m_currentIndex = itA.m_currentIndex;
        ++itB;

        for (/*init above*/; itB.IsValid(); ++itB)
        {
            if (itB.m_currentIndex == itA.m_currentIndex)
            {
                continue;
            }

            Vec2& posB = transStorage.Get(itB.m_currentIndex)->m_pos;
            CCollision& collB = *collStorage.Get(itB.m_currentIndex);
            float& radiusB = collB.m_radius;

            bool ACanPushB = (collA.m_type == CollisionType::Static && collB.m_type == CollisionType::Mobile);
            bool BCanPushA = (collA.m_type == CollisionType::Mobile && collB.m_type == CollisionType::Static);
            bool PushEachOther = (collA.m_type == CollisionType::Mobile && collB.m_type == CollisionType::Mobile);
            if (ACanPushB)
            {
                PushDiscOutOfDisc2D(posB, radiusB, posA, radiusA);
            }
            else if (BCanPushA)
            {
                PushDiscOutOfDisc2D(posA, radiusA, posB, radiusB);
            }
            else if (PushEachOther)
            {
                PushDiscsOutOfEachOther2D(posA, radiusA, posB, radiusB);
            }
        }
    }
}
