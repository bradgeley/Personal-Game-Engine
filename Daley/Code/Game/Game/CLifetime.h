// Bradley Christensen - 2022-2026
#pragma once



//----------------------------------------------------------------------------------------------------------------------
struct CLifetime
{
public:

    CLifetime() = default;
    CLifetime(void const* xmlElement);

	void SetLifetime(float lifetime);

public:

	float m_lifetime            = -1.f;         // < 0 means infinite
	float m_lifetimeRemaining   = -1.f;         // < 0 means infinite
};
