// Bradley Christensen - 2022-2025
#pragma once



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
	float m_outdoorLightTint[4];                  // 16 bytes
	//------------------------------------------------------
	float m_indoorLightTint[4];					// 16 bytes
	//------------------------------------------------------
	float m_ambientLightTint[4];                  // 16 bytes
	//------------------------------------------------------
	float m_ambientLightIntensity;                // 4 bytes
	float padding[3];                           // 12 bytes
	//------------------------------------------------------
};