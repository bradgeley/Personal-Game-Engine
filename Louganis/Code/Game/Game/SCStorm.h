// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
class SCStorm
{
public:

	// Settings
	Vec2  m_initialStormLocation				= Vec2::ZeroVector; // maybe better in world settings
	float m_initialStormMovementSpeed			= 0.25f;    // maybe better in world settings
	float m_initialStormRadius					= 5.f;    // maybe better in world settings
	float m_initialStormRadiusGrowthRate		= 0.1f;   // maybe better in world settings
	float m_initialStormRadiusAccelerationRate	= 0.f;    // maybe better in world settings

	// Runtime values
	Vec2  m_stormLocation						= Vec2::ZeroVector;
	float m_stormMovementSpeed					= 0.f; // units per second
	float m_stormRadius							= 0.f; // units
	float m_stormRadiusGrowthRate				= 0.f; // units per second
	float m_stormRadiusAccelerationRate			= 0.f; // units per second

	VertexBufferID m_stormCircleVertexBuffer = RendererUtils::InvalidID; // Owned by SRenderStorm
};

