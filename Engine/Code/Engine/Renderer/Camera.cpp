// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Camera.h"



//----------------------------------------------------------------------------------------------------------------------
Camera::Camera(Vec3 const& bottomLeft, Vec3 const& topRight) : m_mins(bottomLeft), m_maxs(topRight)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetOrthoDims(Vec3 const& dims)
{
    Vec3 center = GetOrthoCenter();
    Vec3 halfDims = dims * 0.5f;
    m_maxs = center + halfDims;
    m_mins = center - halfDims;
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetOrthoDims2D(Vec2 const& dims2D)
{
    Vec3 center = GetOrthoCenter();
    Vec2 halfDims = dims2D * 0.5f;
    m_maxs.x = center.x + halfDims.x;
    m_maxs.y = center.y + halfDims.y;
    m_mins.x = center.x - halfDims.x;
    m_mins.y = center.y - halfDims.y;
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetOrthoBounds(Vec3 const& mins, Vec3 const& maxs)
{
	m_mins = mins;
	m_maxs = maxs;
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetOrthoCenter(Vec3 const& center)
{
    Vec3 halfDims = GetOrthoHalfDimensions();
    m_maxs = center + halfDims;
    m_maxs = center - halfDims;
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetOrthoCenter2D(Vec2 const& center)
{
    Vec3 halfDims = GetOrthoHalfDimensions();
    m_maxs.x = center.x + halfDims.x;
    m_maxs.y = center.y + halfDims.y;
    m_mins.x = center.x - halfDims.x;
    m_mins.y = center.y - halfDims.y;
}



//----------------------------------------------------------------------------------------------------------------------
Vec3 Camera::GetOrthoCenter() const
{
    return (m_maxs + m_mins) * 0.5f;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Camera::GetOrthoCenter2D() const
{
    return Vec2((m_maxs + m_mins) * 0.5f);
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
Vec3 Camera::GetOrthoHalfDimensions() const
{
    return (m_maxs - m_mins) * 0.5f;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Camera::ScreenToWorldOrtho(Vec2 const& relativeScreenPos) const
{
    return Vec2(m_mins + Vec3(relativeScreenPos) * GetOrthoDimensions());
}
