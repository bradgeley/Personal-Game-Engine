// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/EntityID.h"



//----------------------------------------------------------------------------------------------------------------------
struct CProjectile
{
public:

    CProjectile() = default;
    CProjectile(void const* xmlElement);

public:

    EntityID m_targetID; // todo: add salt to ecs that can verify that a new entity didnt take the place of this id
    float m_accumulatedTime = 0.f;
    float m_damage = 1.f;
    float m_projSpeedUnitsPerSec = 1.f;
};