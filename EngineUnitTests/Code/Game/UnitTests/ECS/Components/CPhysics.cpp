// Bradley Christensen - 2023
#include "CPhysics.h"



//----------------------------------------------------------------------------------------------------------------------
CPhysics::CPhysics( CPhysics const& other )
{
	m_velocity = other.m_velocity;
}



//----------------------------------------------------------------------------------------------------------------------
Component* CPhysics::DeepCopy() const
{
	CPhysics* newComp = new CPhysics;
	*newComp = *this;
	return newComp;
}
