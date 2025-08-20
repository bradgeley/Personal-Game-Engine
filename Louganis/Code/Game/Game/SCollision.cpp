// Bradley Christensen - 2023
#include "SCollision.h"
#include "CCollision.h"
#include "CTransform.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Startup()
{
    AddWriteDependencies<CTransform>();
    AddReadDependencies<CCollision>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Run(SystemContext const& context)
{
    // Naive collision, n^2 complexity

    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collStorage  = g_ecs->GetArrayStorage<CCollision>();

    GroupIter itA = g_ecs->Iterate<CTransform, CCollision>(context);
    GroupIter itB = itA;

    for (/*init above*/; itA.IsValid(); ++itA)
    {
        CTransform& transA = transStorage[itA];
        CCollision& collA = *collStorage.Get(itA);

        Vec2& posA = transA.m_pos;
        float& radiusA = collA.m_radius;

        // Start iterator B at one entity after iterator A (without copying a bunch of data every iteration by calling itB = itA;
        itB.m_currentIndex = itA.m_currentIndex;
        ++itB;

        for (/*init above*/; itB.IsValid(); ++itB)
        {
            CTransform& transB = transStorage[itB];
            CCollision& collB = *collStorage.Get(itB);

            Vec2& posB = transB.m_pos;
            float& radiusB = collB.m_radius;

            GeometryUtils::PushDiscsOutOfEachOther2D(posA, radiusA, posB, radiusB);
        }
    }
}