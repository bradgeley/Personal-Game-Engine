// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/SpriteAnimation.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
enum class DeathTags : uint8_t
{
	IsDead						= 1 << 0,
	DiedThisFrame				= 1 << 1,
	DeathAnimFinished			= 1 << 2,
	DeathAnimFinishedThisFrame	= 1 << 3,
};



//----------------------------------------------------------------------------------------------------------------------
struct CDeath
{
public:

    CDeath() = default;
    CDeath(void const* xmlElement);

	bool GetIsDead() const;
	bool GetDiedThisFrame() const;
	bool GetDeathAnimFinished() const;
	bool GetDeathAnimFinishedThisFrame() const;

	void SetIsDead(bool isDead);
	void SetDiedThisFrame(bool diedThisFrame);
	void SetDeathAnimFinished(bool isFinished);
	void SetDeathAnimFinishedThisFrame(bool isFinished);

public:

	uint8_t m_deathTags				= 0;
	Name m_deathAnimationName		= "Death";
	float m_corpseDurationSeconds	= 5.f; // -1 is infinite, 0 is no corpse
};
