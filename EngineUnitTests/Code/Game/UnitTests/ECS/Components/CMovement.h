// Bradley Christensen - 2023
#pragma once
#include "Engine/Math/Vec2.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class MovementFlag : uint8_t
{
	Velocity		= 1 << 0,
	Acceleration	= 1 << 1,
};



//----------------------------------------------------------------------------------------------------------------------
struct CMovement
{
public:

	CMovement() = default;
	CMovement(float movementSpeed);
	CMovement(CMovement const& other);

	Vec2 m_frameMoveDirection		= Vec2::ZeroVector;
	float m_frameMoveStrength		= 0.f;

	// later move to entity definition? (or movement component definition)
	float m_movementSpeed			= 1.f;
	uint8_t m_movementFlags			= (uint8_t) MovementFlag::Acceleration;
};