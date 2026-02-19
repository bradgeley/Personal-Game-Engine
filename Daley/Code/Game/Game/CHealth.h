// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/TagQuery.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class HealthFlags : uint8_t
{
    // Usually immutable
	IsTargetable            = 1 << 0,
	NeverShowHealthBar      = 1 << 1,

    // Statuses
    Invincible              = 1 << 2,
	RegenSuppressed         = 1 << 3,

    // State
	HealthReachedZero       = 1 << 4,
};



//----------------------------------------------------------------------------------------------------------------------
struct CHealth
{
public:

    CHealth() = default;
    CHealth(void const* xmlElement);

    void TakeDamage(float damage);

    bool MatchesTagQuery(TagQuery query) const;
    bool GetHealthReachedZero() const;
    bool GetIsInvincible() const;
    bool GetNeverShowHealthBar() const;
    bool GetRegenSuppressed() const;
	bool GetIsTargetable() const;

	void SetHealthReachedZero(bool reachedZero);
    void SetInvincible(bool isInvincible);
	void SetNeverShowHealthBar(bool neverShowHealthBar);
	void SetRegenSuppressed(bool regenSuppressed); // maybe replace with a int counter for multiple sources of suppress regen
	void SetIsTargetable(bool isTargetable);

public:
    
	float m_maxHealth       = 100.f;
	float m_currentHealth   = 100.f;
    float m_healthRegen     = 0.f;
    uint8_t m_healthFlags   = false;
};
