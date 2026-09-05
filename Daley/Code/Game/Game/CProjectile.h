// Bradley Christensen - 2022-2026
#pragma once
#include "Ability.h"
#include "HitPayload.h"
#include "Engine/ECS/EntityID.h"
#include "Engine/Math/Vec2.h"
#include <optional>



struct EntityDebugContext;



//----------------------------------------------------------------------------------------------------------------------
struct CProjectile
{
public:

    CProjectile() = default;
    CProjectile(void const* xmlElement);

	HitPayload GetMainTargetPayload() const;
    HitPayload GetAoeTargetPayload() const;

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

    EntityID m_targetID;
	std::optional<Vec2> m_targetPos; // Used if TargetID is invalid. If TargetID is valid, then this is updated to the target's location each frame.
    float m_accumulatedTime = 0.f;
    float m_projSpeed = 1.f;

    RolledOnHitComponent m_onHitComp;
};