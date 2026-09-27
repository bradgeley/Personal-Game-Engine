// Bradley Christensen - 2022-2026
#pragma once
#include "GameCommon.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Math/Vec3.h"
#include "Engine/Math/Vec4.h"
#include "Engine/Renderer/Rgba8.h"
#include <cstdint>
#include <array>



struct InputLayout;



//----------------------------------------------------------------------------------------------------------------------
struct SwirlInstance
{
	//------------------------------------------------------
	Vec3		m_position;			// INSTANCEPOSITION		(12 bytes)
	float		m_orientation;		// INSTANCEROTATION		(4 bytes)
	//------------------------------------------------------
	Rgba8		m_rgba;				// INSTANCETINT			(4 bytes)
	Rgba8		m_outlineRgba;		// INSTANCEOUTLINETINT	(4 bytes)
	Vec2		m_dims;				// INSTANCEDIMS			(8 bytes)
	uint32_t	m_spriteIndex;		// INDEX				(4 byte)
	//------------------------------------------------------
	uint8_t		m_indoorLight;		// INDOORLIGHT			(1 byte)
	uint8_t		m_outdoorLight;		// OUTDOORLIGHT			(1 byte)
	uint8_t		m_numFlavors;		// NUMFLAVORS			(1 byte)
	uint8_t		m_numSwirls;		// NUMSWIRLS			(1 byte) // number of times that each flavor shows up in the swirl
	float		m_swirlRotation;	// SWIRLROTATION		(4 bytes) // Total rotation of the swirl
	uint8_t		m_flavorIndices[StaticGameSettings::s_maxFlavorsInOneTower];	// FLAVORINDICES		(3-4? bytes)
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
struct FlavorConstants
{
	static int GetSlot() { return 8; }

	//------------------------------------------------------
	std::array<Vec4, 32> m_flavorTints; // FLAVORTINTS (384 bytes)
	//------------------------------------------------------
};