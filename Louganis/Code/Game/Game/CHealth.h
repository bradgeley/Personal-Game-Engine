// Bradley Christensen - 2022-2025
#pragma once
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class HealthFlags : uint8_t
{
    None                    = 0,
    Invincible              = 1 << 0,
	NeverShowHealthBar      = 1 << 1,
};



//----------------------------------------------------------------------------------------------------------------------
struct CHealth
{
public:

    CHealth() = default;
    CHealth(void const* xmlElement);

    bool GetIsDead() const;
    bool GetIsInvincible() const;
    bool GetNeverShowHealthBar() const;

    void SetInvincible(bool isInvincible);
	void SetNeverShowHealthBar(bool neverShowHealthBar);

public:
    
	float m_maxHealth = 100.f;
	float m_currentHealth = 100.f;
    float m_healthRegen = 0.f;
    uint8_t m_healthFlags = false;
};
