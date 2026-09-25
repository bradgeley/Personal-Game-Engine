// Bradley Christensen - 2022-2026
#pragma once
#include "HitPayload.h"
#include "Engine/Renderer/Rgba8.h"



struct AbilityAoEEffectComponent;
struct RolledAoEEffectComponent;



//----------------------------------------------------------------------------------------------------------------------
struct CCollisionEffect
{
public:

	CCollisionEffect() = default;

	void Initialize(RolledAoEEffectComponent const& aoeEffectComp);

	HitPayload GetWhileOverlappingPayload(float deltaSeconds) const;

public:

	HitPayload m_payloadPerSecond;

	// Rendering (uses radius and position from Collision Component)
	Rgba8 m_tint				= Rgba8::TransparentWhite;
	float m_depth				= 0.f;
};
