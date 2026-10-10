// Bradley Christensen - 2022-2026
#pragma once
#include "HitPayload.h"
#include "Engine/Renderer/Rgba8.h"



struct AbilityPuddleComponent;
struct RolledPuddleComponent;



//----------------------------------------------------------------------------------------------------------------------
struct CPuddle
{
public:

	CPuddle() = default;

	void Initialize(RolledPuddleComponent const& puddleComp);

	HitPayload GetWhileOverlappingPayload(float deltaSeconds) const;

public:

	HitPayload m_payloadPerSecond;

	// Rendering (uses radius and position from Collision Component)
	Rgba8 m_tint				= Rgba8::TransparentWhite;
	float m_depth				= 0.f;
};
