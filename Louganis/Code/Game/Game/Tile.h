// Bradley Christensen - 2022-2025
#pragma once
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class TileFlags
{

};



//----------------------------------------------------------------------------------------------------------------------
struct Tile
{
	uint8_t m_id			= 0;
	uint8_t m_lighting		= 0; // 4 bits indoor (XXXX0000), 4 outdoor (0000XXXX)
	uint8_t m_flags			= 0;	
};