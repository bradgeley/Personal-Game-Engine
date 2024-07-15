﻿// Bradley Christensen - 2023
#include "SCollision.h"
#include "CCollision.h"
#include "CTransform.h"
#include "Engine/Input/InputSystem.h"
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

    auto itA = g_ecs->Iterate<CTransform, CCollision>(context);
    auto itB = itA;

    for (/*init above*/; itA.IsValid(); ++itA)
    {
        CTransform& transA = transStorage[itA.m_currentIndex];
        CCollision& collA = *collStorage.Get(itA.m_currentIndex);

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

            PushDiscsOutOfEachOther2D(posA, radiusA, posB, radiusB);
        }
    }
}