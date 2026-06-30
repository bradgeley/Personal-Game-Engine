// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/Vec3.h"
#include "Engine/Renderer/Rgba8.h"
#include <cstdint>



struct InputLayout;



//----------------------------------------------------------------------------------------------------------------------
struct alignas(16) DiscRenderInstance
{
	//------------------------------------------------------
	Vec3		m_position;			// POSITION		(12 bytes)
	float		m_radius;			// RADIUS		(4 bytes)
	//------------------------------------------------------
	Rgba8		m_tint;				// TINT			(4 bytes)

	float padding[3];				// PADDING		(12 bytes)
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
struct alignas(16) DiscRenderConstants
{
	static int GetSlot() { return 5; }

	//------------------------------------------------------
	uint32_t m_numSides;			// NUMSIDES		(4 bytes) // must be power of 2 between 4 and 64

	float padding[3];				// PADDING		(12 bytes)
	//------------------------------------------------------
};