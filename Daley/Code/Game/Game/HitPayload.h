// Bradley Christensen - 2022-2026
#pragma once



//----------------------------------------------------------------------------------------------------------------------
struct HitPayload
{
	bool HasValue() const { return m_damage > 0.f || m_burn > 0.f || m_poison > 0.f || m_slowDuration > 0.f; }

	float m_damage = 0.f;
	float m_burn = 0.f;
	float m_poison = 0.f;
	float m_slowDuration = 0.f;
};