// Bradley Christensen - 2022-2025
#pragma once
#include "WorldCoords.h"



//----------------------------------------------------------------------------------------------------------------------
struct TileGeneratedData
{
	float m_terrainHeight			= 0.f; // 0 to 1
	float m_terrainHeightOffset		= 0.f; // -0.5 to +0.5
	float m_mountainness			= 0.f; // 0 to 1
	float m_humidity				= 0.f; // 0 to 1
	float m_oceanness				= 0.f; // 0 to 1
	float m_riverness				= 0.f; // 0 to 1
	float m_forestness				= 0.f; // 0 to 1
};