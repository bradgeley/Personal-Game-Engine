// Bradley Christensen - 2022-2026
#pragma once
#include <cstdint>



struct EntityDebugContext;



//----------------------------------------------------------------------------------------------------------------------
enum class HitPayloadFlags : uint8_t
{
	DidCrit				= 1 << 0,
	MultiplyByMaxHealth = 1 << 1,
	IgnoreResistances	= 1 << 2,
};



//----------------------------------------------------------------------------------------------------------------------
struct HitPayload
{
	bool HasValue() const { return m_physical != 0.f || m_burn != 0.f || m_poison != 0.f || m_vulnerability != 0.f || m_slowDuration != 0.f || m_hasteDuration != 0.f; }
	bool IsRelevantToHealth() const { return m_physical != 0.f || m_burn != 0.f || m_poison != 0.f || m_vulnerability != 0.f; }
	bool IsRelevantToTime() const { return m_slowDuration != 0.f || m_hasteDuration != 0.f; }
	void AppendDebugString(EntityDebugContext& debugContext) const;

	void SetDidCrit(bool didCrit);
	void SetMultiplyByMaxHealth(bool multiplyByMaxHealth);
	void SetIgnoreResistances(bool ignoreResistances);
	bool GetDidCrit() const;
	bool GetMultiplyByMaxHealth() const;
	bool GetIgnoreResistances() const;

	float m_physical = 0.f;
	float m_burn = 0.f;
	float m_poison = 0.f;
	float m_vulnerability = 0.f;
	float m_slowDuration = 0.f;
	float m_hasteDuration = 0.f;
	uint8_t m_flags = 0;

	void operator*=(float multiplier)
	{
		m_physical *= multiplier;
		m_burn *= multiplier;
		m_poison *= multiplier;
		m_vulnerability *= multiplier;
		m_slowDuration *= multiplier;
		m_hasteDuration *= multiplier;
	}
};