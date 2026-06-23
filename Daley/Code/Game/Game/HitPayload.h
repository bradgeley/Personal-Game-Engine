// Bradley Christensen - 2022-2026
#pragma once



//----------------------------------------------------------------------------------------------------------------------
struct HitPayload
{
	bool HasValue() const { return m_damage > 0.f || m_burn > 0.f || m_poison > 0.f || m_slowDuration > 0.f; }
	bool IsRelevantToHealth() { return m_damage != 0.f || m_burn != 0.f || m_poison != 0.f; }
	bool IsRelevantToTime() { return m_slowDuration != 0.f; }

	float m_damage = 0.f;
	float m_burn = 0.f;
	float m_poison = 0.f;
	float m_slowDuration = 0.f;

	void operator+=(HitPayload const& other)
	{
		m_damage += other.m_damage;
		m_burn += other.m_burn;
		m_poison += other.m_poison;
		m_slowDuration += other.m_slowDuration;
	}

	void operator*=(float multiplier)
	{
		m_damage *= multiplier;
		m_burn *= multiplier;
		m_poison *= multiplier;
		m_slowDuration *= multiplier;
	}
};