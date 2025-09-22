// Bradley Christensen - 2022-2025
#include "TileGeneratedData.h"
#include "Engine/Core/StringUtils.h"



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
	return result;
}