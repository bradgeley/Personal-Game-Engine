// Bradley Christensen - 2023
#pragma once
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct CPhysics
{
public:

	CPhysics() = default;
	CPhysics( CPhysics const& other );

	Vec2 m_velocity					= Vec2::ZeroVector;
	Vec2 m_acceleration				= Vec2::ZeroVector;
	float m_angularVelocity			= 0.f;
};