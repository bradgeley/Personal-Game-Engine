// Bradley Christensen - 2023
#include "CTransform2D.h"



//----------------------------------------------------------------------------------------------------------------------
CTransform2D::CTransform2D(CTransform2D const& other)
{
    m_screenPosition = other.m_screenPosition;
    m_orientationDegrees = other.m_orientationDegrees;
}



//----------------------------------------------------------------------------------------------------------------------
CTransform2D::CTransform2D(Vec2 const& v, float orientationDegrees) : m_screenPosition(v), m_orientationDegrees(orientationDegrees)
{

}



//----------------------------------------------------------------------------------------------------------------------
CTransform2D::CTransform2D(float x, float y, float orientationDegrees) : m_screenPosition(x, y), m_orientationDegrees(orientationDegrees)
{

}