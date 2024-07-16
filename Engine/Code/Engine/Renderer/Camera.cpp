// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Camera.h"



//----------------------------------------------------------------------------------------------------------------------
Camera::Camera(Vec3 const& bottomLeft, Vec3 const& topRight) : m_mins(bottomLeft), m_maxs(topRight)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
Camera::Camera(float minX, float minY, float maxX, float maxY) : Camera(minX, minY, 0.f, maxX, maxY, 0.f)
{

}



//----------------------------------------------------------------------------------------------------------------------
Camera::Camera(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) : m_mins(minX, minY, minZ), m_maxs(maxX, maxY, maxZ)
{

}



//----------------------------------------------------------------------------------------------------------------------
void Camera::DefineGameSpace(Vec3 const& gameForward, Vec3 const& gameLeft, Vec3 const& gameUp)
{
    // D3D11 is x-right y-up z-forward
    // If we want x to be forward, we need to create a matrix that transforms x values into z values.
    // We do this by creating a matrix where the Z (forward) vector is our new X vector, and get the inverse by transposing it (all 3 inputs must be orthonormal)
    // Then any points we pass through this matrix will be transformed from X forward to Z forward for rendering.
    m_gameToRenderTransform.SetIJK(-gameLeft, gameUp, gameForward);
    m_gameToRenderTransform.Transpose();
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetPosition(Vec3 const& position)
{
    m_position = position;
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetPosition2D(Vec2 const& position)
{
    m_position = Vec3(position, 0.f);
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::Translate(Vec3 const& deltaPos)
{
    m_position += deltaPos;
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::Translate2D(Vec2 const& deltaPos)
{
    m_position += Vec3(deltaPos, 0.f);
}




//----------------------------------------------------------------------------------------------------------------------
void Camera::SetRotation2D(float rotation)
{
    m_rotation2D = rotation;
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
void Camera::SetOrthoBounds2D(AABB2 const& orthoBounds)
{
    m_mins.x = orthoBounds.mins.x;
    m_mins.y = orthoBounds.mins.y;
    m_maxs.x = orthoBounds.maxs.x;
    m_maxs.y = orthoBounds.maxs.y;
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
void Camera::ZoomAroundCenter2D(float zoomRatio, Vec2 const& center)
{
    Vec2 mins2D = Vec2(m_mins);
    Vec2 maxs2D = Vec2(m_maxs);
    mins2D -= center;
    maxs2D -= center;
    mins2D *= zoomRatio;
    maxs2D *= zoomRatio;
    mins2D += center;
    maxs2D += center;
    m_mins = Vec3(mins2D, m_mins.z);
    m_maxs = Vec3(maxs2D, m_maxs.z);
}



//----------------------------------------------------------------------------------------------------------------------
Vec3 const& Camera::GetPosition() const
{
    return m_position;
}



//----------------------------------------------------------------------------------------------------------------------
float Camera::GetRotation2D() const
{
    return m_rotation2D;
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



//----------------------------------------------------------------------------------------------------------------------
CameraConstants Camera::GetCameraConstants() const
{
    CameraConstants result;
    result.m_worldToCamera = CalculateViewMatrix();
    result.m_cameraToClip = CalculateOrthoProjectionMatrix();
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
Mat44 Camera::CalculateViewMatrix() const
{ 
    Mat44 cameraToWorld = Mat44::CreateZRotationDegrees(m_rotation2D);
    cameraToWorld.SetTranslation3D(m_position);
    return cameraToWorld.GetOrthoNormalInverse();
}



//----------------------------------------------------------------------------------------------------------------------
Mat44 Camera::CalculateOrthoProjectionMatrix() const
{
    return Mat44::CreateOrthoProjection(m_mins.x, m_maxs.x, m_mins.y, m_maxs.y, m_mins.z, m_maxs.z);
}
