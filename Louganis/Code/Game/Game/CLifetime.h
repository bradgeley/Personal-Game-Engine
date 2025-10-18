// Bradley Christensen - 2022-2025
#pragma once



//----------------------------------------------------------------------------------------------------------------------
struct CLifetime
{
    CLifetime() = default;
    CLifetime(void const* xmlElement);

	void SetLifetime(float lifetime);
    
	float m_lifetime            = -1.f;         // < 0 means infinite
	float m_lifetimeRemaining   = -1.f;         // < 0 means infinite
};
