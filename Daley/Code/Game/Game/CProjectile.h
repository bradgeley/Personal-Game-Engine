// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/EntityID.h"
#include "Engine/Math/Vec2.h"
#include <optional>



//----------------------------------------------------------------------------------------------------------------------
struct SplashDamageProjComp
{
    float m_splashRadius = 0.f;
	float m_splashDamage = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct CProjectile
{
public:

    CProjectile() = default;
    CProjectile(void const* xmlElement);

public:

    EntityID m_targetID;
	std::optional<Vec2> m_targetPos; // Used if TargetID is invalid. If TargetID is valid, then this is updated to the target's location each frame.
    float m_accumulatedTime = 0.f;
    float m_damage = 1.f;
    float m_projSpeedUnitsPerSec = 1.f;

    std::optional<SplashDamageProjComp> m_splashComp;
};