// Bradley Christensen - 2023
#include "CMovement.h"



//----------------------------------------------------------------------------------------------------------------------
CMovement::CMovement( CMovement const& other )
{
	m_movementSpeed = other.m_movementSpeed;
	m_movementFlags = other.m_movementFlags;
}



//----------------------------------------------------------------------------------------------------------------------
CMovement::CMovement( float movementSpeed )
{
	m_movementSpeed = movementSpeed;
}