// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/Vec3.h"
#include "Engine/Math/IntVec2.h"
#include <cstdint>



struct InputLayout;



//----------------------------------------------------------------------------------------------------------------------
struct alignas(16) SpriteInstance
{
	//------------------------------------------------------
	Vec3		m_position;		// POSITION		(12 bytes)
	float		m_orientation;	// ORIENTATION	(4 bytes)
	//------------------------------------------------------
	float		m_scale;		// SCALE		(4 bytes)
	uint32_t	m_rgba;			// TINT			(4 bytes)
	uint32_t	m_spriteIndex;	// SPRITEINDEX	(4 bytes)
	uint32_t	m_padding;		// PADDING		(4 bytes)
	//------------------------------------------------------

	static InputLayout* GetInputLayout();
};



//----------------------------------------------------------------------------------------------------------------------
struct SpriteSheetConstants
{
	//------------------------------------------------------
	IntVec2	m_layout;			// LAYOUT		(8 bytes)
	IntVec2	m_edgePadding;		// EDGEPADDING	(8 bytes)
	//------------------------------------------------------
	IntVec2	m_innerPadding;		// INNERPADDING	(8 bytes)
	float	padding[2];			// PADDING		(8 bytes)
	//------------------------------------------------------
};