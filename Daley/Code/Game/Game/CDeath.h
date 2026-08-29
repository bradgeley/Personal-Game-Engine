// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class DeathTags : uint8_t
{
	IsDead						= 1 << 0,
	DiedThisFrame				= 1 << 1,
};



//----------------------------------------------------------------------------------------------------------------------
struct CDeath
{
public:

    CDeath() = default;
    CDeath(void const* xmlElement);

	bool GetIsDead() const;
	bool GetDiedThisFrame() const;

	void SetIsDead(bool isDead);
	void SetDiedThisFrame(bool diedThisFrame);

public:

	uint8_t m_deathFlags			= 0;
	Name m_deathAnimationName		= "Death";
	float m_corpseDurationSeconds	= 1.f; // -1 is infinite, 0 is no corpse
	float m_goldReward				= 0.f;
	uint32_t m_expReward			= 0;
};
