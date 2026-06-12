// Bradley Christensen - 2022-2026
#pragma once
#include "HitPayload.h"



//----------------------------------------------------------------------------------------------------------------------
struct CCollisionEffect
{
public:

	CCollisionEffect() = default;

	HitPayload GetFirstOverlapPayload() const;
	HitPayload GetWhileOverlappingPayload(float deltaSeconds) const;

public:

	// First overlap effects
	float m_firstOverlapDamage	= 0.f;
	float m_firstOverlapBurn	= 0.f;
	float m_firstOverlapPoison	= 0.f;
	float m_firstOverlapSlow	= 0.f;

	// While overlapping effects
	float m_damagePerSecond		= 0.f;
	float m_burnPerSecond		= 0.f;
	float m_poisonPerSecond		= 0.f;
	float m_slowPerSecond		= 0.f;
};
