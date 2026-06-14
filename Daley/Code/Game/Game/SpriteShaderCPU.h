// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/Vec3.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Renderer/Rgba8.h"
#include <cstdint>



struct InputLayout;



//----------------------------------------------------------------------------------------------------------------------
struct alignas(16) SpriteInstance
{
	//------------------------------------------------------
	Vec3		m_position;			// INSTANCEPOSITION	(12 bytes)
	float		m_orientation;		// INSTANCEROTATION	(4 bytes)
	//------------------------------------------------------
	Rgba8		m_rgba;				// INSTANCETINT		(4 bytes -> 16 bytes as float4)
	//------------------------------------------------------
	Vec2		m_dims;				// INSTANCEDIMS		(8 bytes)
	uint32_t	m_spriteIndex;		// INDEX			(4 byte)
	uint8_t		m_indoorLight;		// INDOORLIGHT		(1 byte)
	uint8_t		m_outdoorLight;		// OUTDOORLIGHT		(1 byte)
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
struct SpriteSheetConstants
{
	//------------------------------------------------------
	IntVec2	m_layout;			// LAYOUT		(8 bytes)
	IntVec2	m_edgePadding;		// EDGEPADDING	(8 bytes)
	//------------------------------------------------------
	IntVec2	m_innerPadding;		// INNERPADDING	(8 bytes)
	IntVec2	m_textureDims;		// TEXTUREDIMS	(8 bytes)
	//------------------------------------------------------
};