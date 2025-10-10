// Bradley Christensen - 2022-2025
#include "TileGeneratedData.h"
#include "Engine/Core/StringUtils.h"
#include "EntityDef.h"



//----------------------------------------------------------------------------------------------------------------------
std::string TileGeneratedData::ToString() const
{
	// m_terrainHeight = 0.f; // 0 to 1
	// m_terrainHeightOffset = 0.f; // -0.5 to +0.5
	// m_mountainness = 0.f; // 0 to 1
	// m_humidity = 0.f; // 0 to 1
	// m_oceanness = 0.f; // 0 to 1
	// m_riverness = 0.f; // 0 to 1
	// m_forestness = 0.f; // 0 to 1
	// m_islandness = 0.f; // 0 to 1
	// m_temperature = 0.f; // 0 to 1
	std::string result;
	result += StringUtils::StringF("TerrainHeight: %.3f\n", m_terrainHeight);
	result += StringUtils::StringF("TerrainHeightOffset: %.3f\n", m_terrainHeightOffset);
	result += StringUtils::StringF("Mountainness: %.3f\n", m_mountainness);
	result += StringUtils::StringF("Humidity: %.3f\n", m_humidity);
	result += StringUtils::StringF("Oceanness: %.3f\n", m_oceanness);
	result += StringUtils::StringF("Riverness: %.3f\n", m_riverness);
	result += StringUtils::StringF("Forestness: %.3f\n", m_forestness);
	result += StringUtils::StringF("Islandness: %.3f\n", m_islandness);
	result += StringUtils::StringF("Temperature: %.3f\n", m_temperature);
	result += StringUtils::StringF("Treeness: %.3f\n", m_treeness);
	return result;
}

std::string TileGeneratedData::ToStringVerbose() const
{
	std::string result;

	result += StringUtils::StringF("TerrainHeight: %.3f\n", m_terrainHeight);
	result += StringUtils::StringF("TerrainHeightOffset: %.3f\n", m_terrainHeightOffset);
	result += StringUtils::StringF("Mountainness: %.3f\n", m_mountainness);
	result += StringUtils::StringF("Humidity: %.3f\n", m_humidity);
	result += StringUtils::StringF("Oceanness: %.3f\n", m_oceanness);
	result += StringUtils::StringF("Riverness: %.3f\n", m_riverness);
	result += StringUtils::StringF("Forestness: %.3f\n", m_forestness);
	result += StringUtils::StringF("Islandness: %.3f\n", m_islandness);
	result += StringUtils::StringF("Temperature: %.3f\n", m_temperature);
	result += StringUtils::StringF("Treeness: %.3f\n", m_treeness);
	result += StringUtils::StringF("TreeDef: %s\n", m_treeDef == nullptr ? "null" : m_treeDef->m_name.ToCStr());
	result += StringUtils::StringF("Static Lighting (0-1): %.3f\n", m_staticLighting01);

	result += StringUtils::StringF("IsIsland: %s\n", m_isIsland ? "true" : "false");
	result += StringUtils::StringF("IsRiver: %s\n", m_isRiver ? "true" : "false");
	result += StringUtils::StringF("IsOcean: %s\n", m_isOcean ? "true" : "false");
	result += StringUtils::StringF("IsOceanSand: %s\n", m_isOceanSand ? "true" : "false");
	result += StringUtils::StringF("IsShallowOcean: %s\n", m_isShallowOcean ? "true" : "false");
	result += StringUtils::StringF("IsMediumOcean: %s\n", m_isMediumOcean ? "true" : "false");
	result += StringUtils::StringF("IsDeepOcean: %s\n", m_isDeepOcean ? "true" : "false");
	result += StringUtils::StringF("IsDesert: %s\n", m_isDesert ? "true" : "false");
	result += StringUtils::StringF("IsForest: %s\n", m_isForest ? "true" : "false");
	result += StringUtils::StringF("IsDeepForest: %s\n", m_isDeepForest ? "true" : "false");
	result += StringUtils::StringF("IsCold: %s\n", m_isCold ? "true" : "false");
	result += StringUtils::StringF("CanGrowTrees: %s\n", m_canGrowTrees ? "true" : "false");

	return result;
}
