// Bradley Christensen - 2023
#include "CMovement.h"



//----------------------------------------------------------------------------------------------------------------------
CMovement::CMovement( CMovement const& other )
{
	m_movementSpeed = other.m_movementSpeed;
	m_movementFlags = other.m_movementFlags;
}



//----------------------------------------------------------------------------------------------------------------------
Component* CMovement::DeepCopy() const
{
	CMovement* newComp = new CMovement;
	*newComp = *this;
	return newComp;
}



//----------------------------------------------------------------------------------------------------------------------
CMovement::CMovement( float movementSpeed )
{
	m_movementSpeed = movementSpeed;
}