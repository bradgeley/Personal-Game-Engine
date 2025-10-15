// Bradley Christensen - 2022-2025
#pragma once



//----------------------------------------------------------------------------------------------------------------------
struct CHealth
{
    CHealth() = default;
    CHealth(void const* xmlElement);
    
	float m_maxHealth = 100.f;
	float m_currentHealth = 100.f;
    float m_healthRegen = 0.f;
};
