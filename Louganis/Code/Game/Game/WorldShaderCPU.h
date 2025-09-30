// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/Vec3.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/Rgba8.h"



struct InputLayout;



//----------------------------------------------------------------------------------------------------------------------
struct TerrainVertex
{
public:

	TerrainVertex() = default;
	TerrainVertex(Vec3 const& pos, Rgba8 const& tint, Vec2 const& uvs, Vec2 const& lightmapUVs);

	Vec3    m_pos;
	Rgba8   m_tint;
	Vec2    m_uvs;
	Vec2    m_lightmapUVs;
};



//----------------------------------------------------------------------------------------------------------------------
struct alignas(16) StaticWorldConstants // : register(b6)
{
	static int GetSlot() { return 6; }

	//------------------------------------------------------
	float	m_chunkWidth;			            // 4 bytes
	float	m_tileWidth;		                // 4 bytes
	int     m_numTilesInRow;					// 4 bytes
	float padding;
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
struct alignas(16) LightingConstants // : register(b7)
{
	static int GetSlot() { return 7; }

	//------------------------------------------------------
	float m_outdoorLightTint[4];                // 16 bytes
	//------------------------------------------------------
	float m_indoorLightTint[4];					// 16 bytes
	//------------------------------------------------------
	float m_ambientLightTint[4];                // 16 bytes
	//------------------------------------------------------
	float m_ambientLightIntensity;              // 4 bytes
	uint32_t m_isLightingEnabled;				// 4 bytes
	float padding[2];							// 8 bytes
	//------------------------------------------------------
};

constexpr int boolsize = sizeof(bool);