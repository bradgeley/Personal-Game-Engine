// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/Vec2.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
struct alignas(16) HealthBarInstance
{
	//------------------------------------------------------
	Vec2		m_position;			// POSITION			(8 bytes)
	Vec2		m_dimensions;		// DIMENSIONS		(8 bytes)
	//------------------------------------------------------
	float		m_healthFraction;	// HEALTHFRACTION	(4 byte)
	float		m_fireFraction;		// FIREFRACTION		(4 byte)
	float		m_poisonFraction;	// POISONFRACTION	(4 byte)	
	uint32_t 	m_iconMask;			// ICONMASK			(4 bytes)
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
struct HealthBarRenderConstants
{
	static int GetSlot() { return 6; }

	//------------------------------------------------------
	float	m_backgroundTint[4];	// BACKGROUNDTINT	(16 bytes)
	float	m_healthTint[4];		// HEALTHTINT		(16 bytes)
	//------------------------------------------------------
	float	m_fireTint[4];			// FIRETINT			(16 bytes)
	float	m_poisonTint[4];		// POISONTINT		(16 bytes)
	//------------------------------------------------------
};