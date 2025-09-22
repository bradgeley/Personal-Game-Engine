// Bradley Christensen - 2022-2025
#pragma once
#include "WorldCoords.h"
#include <string>



//----------------------------------------------------------------------------------------------------------------------
struct TileGeneratedData
{
	std::string ToString() const;

	float	m_terrainHeight				= 0.f; // 0 to 1
	float	m_terrainHeightOffset		= 0.f; // -0.5 to +0.5
	float	m_mountainness				= 0.f; // 0 to 1
	float	m_humidity					= 0.f; // 0 to 1
	float	m_oceanness					= 0.f; // 0 to 1
	float	m_riverness					= 0.f; // 0 to 1
	float	m_forestness				= 0.f; // 0 to 1
	float	m_islandness				= 0.f; // 0 to 1
	float	m_temperature				= 0.f; // 0 to 1
	float	m_treeness					= 0.f; // 0 to 1
	float	m_treeScale					= 1.f; // non-zero positive

	// Cached bools
	bool	m_isIsland					= false;
	bool	m_isRiver					= false;
	bool	m_isOcean					= false;
	bool	m_isOceanSand				= false;
	bool	m_isShallowOcean			= false;
	bool	m_isMediumOcean				= false;
	bool	m_isDeepOcean				= false;
	bool	m_isDesert					= false;
	bool	m_isForest					= false;
	bool	m_isDeepForest				= false;
	bool	m_isCold					= false;
	bool	m_canGrowTrees				= false;
	bool	m_hasTree					= false;
};