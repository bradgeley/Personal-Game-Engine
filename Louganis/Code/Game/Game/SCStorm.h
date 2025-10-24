// Bradley Christensen - 2022-2025
#pragma once



//----------------------------------------------------------------------------------------------------------------------
class SCStorm
{
public:

	// Settings
	float m_initialStormRadius					= 0.f;   // maybe better in world settings
	float m_initialStormRadiusGrowthRate		= 0.1f;  // maybe better in world settings

	// Runtime values
	float m_stormRadius							= 0.f;
	float m_stormRadiusGrowthRate				= 0.1f; // units per second
};

