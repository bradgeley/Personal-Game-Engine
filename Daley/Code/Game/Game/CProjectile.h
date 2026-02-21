// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/EntityID.h"
#include "Engine/Math/Vec2.h"
#include <optional>



//----------------------------------------------------------------------------------------------------------------------
struct CProjectile
{
public:

    CProjectile() = default;
    CProjectile(void const* xmlElement);

public:

    EntityID m_targetID; // todo: add salt to ecs that can verify that a new entity didnt take the place of this id
	std::optional<Vec2> m_targetPos; // Used if TargetID is invalid
    float m_accumulatedTime = 0.f;
    float m_damage = 1.f;
    float m_projSpeedUnitsPerSec = 1.f;
};