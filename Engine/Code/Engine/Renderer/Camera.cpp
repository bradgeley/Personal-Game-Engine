// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Camera.h"



//----------------------------------------------------------------------------------------------------------------------
Camera::Camera(Vec3 const& bottomLeft, Vec3 const& topRight) : m_mins(bottomLeft), m_maxs(topRight)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
Mat44 Camera::GetOrthoProjectionMatrix() const
{
    return Mat44::CreateOrthoProjection(m_mins.x, m_maxs.x, m_mins.y, m_maxs.y, m_mins.z, m_maxs.z);
}
