// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct CPhysics : Component
{
public:

	CPhysics() = default;
	CPhysics(CPhysics const& other);
	
	Component* DeepCopy() const override;

	Vec2 m_velocity					= Vec2::ZeroVector;
	Vec2 m_acceleration				= Vec2::ZeroVector;
	float m_angularVelocity			= 0.f;
};