// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/Vec2.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Renderer/Rgba8.h"
#include <cstdint>



struct InputLayout;



//----------------------------------------------------------------------------------------------------------------------
struct alignas(16) HealthBarInstance
{
	//------------------------------------------------------
	Vec2		m_position;			// POSITION			(8 bytes)
	Vec2		m_dimensions;		// DIMENSIONS		(8 bytes)
	//------------------------------------------------------
	float		m_healthFraction;	// HEALTHFRACTION	(1 byte)
	float		m_fireFraction;		// FIREFRACTION		(1 byte)
	float		m_poisonFraction;	// POISONFRACTION	(1 byte)	
	uint8_t 	padding[1] = { 0 };	// PADDING			(1 bytes)
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
struct HealthBarRenderConstants
{
	static int GetSlot() { return 5; }

	//------------------------------------------------------
	float	m_backgroundTint[4];	// BACKGROUNDTINT	(16 bytes)
	float	m_healthTint[4];		// HEALTHTINT		(16 bytes)
	//------------------------------------------------------
	float	m_fireTint[4];			// FIRETINT			(16 bytes)
	float	m_poisonTint[4];		// POISONTINT		(16 bytes)
	//------------------------------------------------------
};