// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Camera.h"



//----------------------------------------------------------------------------------------------------------------------
Camera::Camera(Vec3 const& bottomLeft, Vec3 const& topRight) : m_mins(bottomLeft), m_maxs(topRight)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 Camera::GetOrthoBounds2D() const
{
    AABB2 result;
    result.mins = Vec2(m_mins);
    result.maxs = Vec2(m_maxs);
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
Mat44 Camera::GetOrthoProjectionMatrix() const
{
    return Mat44::CreateOrthoProjection(m_mins.x, m_maxs.x, m_mins.y, m_maxs.y, m_mins.z, m_maxs.z);
}



//----------------------------------------------------------------------------------------------------------------------
Vec3 Camera::GetOrthoDimensions() const
{
    return m_maxs - m_mins;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Camera::ScreenToWorldOrtho(Vec2 const& relativeScreenPos) const
{
    return Vec2(m_mins + Vec3(relativeScreenPos) * GetOrthoDimensions());
}
