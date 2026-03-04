// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/EntityID.h"
#include "Engine/Math/Vec2.h"
#include "HitPayload.h"
#include "Ability.h"
#include <optional>



//----------------------------------------------------------------------------------------------------------------------
struct CProjectile
{
public:

    CProjectile() = default;
    CProjectile(void const* xmlElement);

    void RollDamageAndEffects();
	HitPayload GetMainTargetPayload() const;
    HitPayload GetAoeTargetPayload() const;

	void AppendDebugString(std::string& out_string) const;

public:

    EntityID m_targetID;
	std::optional<Vec2> m_targetPos; // Used if TargetID is invalid. If TargetID is valid, then this is updated to the target's location each frame.
    float m_accumulatedTime = 0.f;
    float m_projSpeed = 1.f;

	bool m_hasRolledDamageAndEffects = false;
    std::optional<AbilityCritComponent>	m_critComp;
    std::optional<AbilityOnHitComponent> m_onHitComp;
};