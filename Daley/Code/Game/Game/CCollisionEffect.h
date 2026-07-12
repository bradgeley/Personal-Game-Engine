// Bradley Christensen - 2022-2026
#pragma once
#include "HitPayload.h"
#include "Engine/Renderer/Rgba8.h"



struct AbilityAoEEffectComponent;



//----------------------------------------------------------------------------------------------------------------------
struct CCollisionEffect
{
public:

	CCollisionEffect() = default;

	void InitializeFromAoEEffect(AbilityAoEEffectComponent const& aoeEffectComp);

	HitPayload GetWhileOverlappingPayload(float deltaSeconds) const;

public:

	// While overlapping effects
	float m_damagePerSecond		= 0.f;
	float m_burnPerSecond		= 0.f;
	float m_poisonPerSecond		= 0.f;
	float m_slowPerSecond		= 0.f;

	// Rendering (uses radius and position from Collision Component)
	bool m_shouldRender			= false;
	Rgba8 m_tint				= Rgba8::White;
	float m_depth				= 0.f;
};
