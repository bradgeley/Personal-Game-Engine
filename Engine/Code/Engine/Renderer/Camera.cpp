// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Camera.h"



//----------------------------------------------------------------------------------------------------------------------
Camera::Camera(Vec3 const& bottomLeft, Vec3 const& topRight) : m_mins(bottomLeft), m_maxs(topRight)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::DefineGameSpace(Vec3 const& gameForward, Vec3 const& gameLeft, Vec3 const& gameUp)
{
    // D3D11 is x-right y-up z-forward
    // If we want x to be forward, we need to create a matrix that transforms x values into z values.
    // We do this by creating a matrix where the Z (forward) vector is our new X vector, and get the inverse by transposing it.
    // Then any points we pass through this matrix will be transformed from X forward to Z forward for rendering.
    m_cameraConstants.m_gameToRender.SetIJK(-gameLeft, gameUp, gameForward);
    m_cameraConstants.m_gameToRender.Transpose();
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetPosition(Vec3 const& position)
{
    m_viewMatrixDirty = true;
    m_position = position;
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetRotation2D(float rotation)
{
    m_viewMatrixDirty = true;
    m_rotation2D = rotation;
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetOrthoDims(Vec3 const& dims)
{
    m_projMatrixDirty = true;
    Vec3 center = GetOrthoCenter();
    Vec3 halfDims = dims * 0.5f;
    m_maxs = center + halfDims;
    m_mins = center - halfDims;
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetOrthoDims2D(Vec2 const& dims2D)
{
    m_projMatrixDirty = true;
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
    m_projMatrixDirty = true;
	m_mins = mins;
	m_maxs = maxs;
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetOrthoCenter(Vec3 const& center)
{
    m_projMatrixDirty = true;
    Vec3 halfDims = GetOrthoHalfDimensions();
    m_maxs = center + halfDims;
    m_maxs = center - halfDims;
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetOrthoCenter2D(Vec2 const& center)
{
    m_projMatrixDirty = true;
    Vec3 halfDims = GetOrthoHalfDimensions();
    m_maxs.x = center.x + halfDims.x;
    m_maxs.y = center.y + halfDims.y;
    m_mins.x = center.x - halfDims.x;
    m_mins.y = center.y - halfDims.y;
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::SetCameraConstants(CameraConstants const& cc)
{
    m_cameraConstants = cc;
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
CameraConstants const& Camera::GetCameraConstants() const
{
    UpdateViewMatrix();
    UpdateProjMatrix();
    return m_cameraConstants;
}



//----------------------------------------------------------------------------------------------------------------------
Mat44 Camera::GetViewMatrix() const
{
    UpdateViewMatrix();
    return m_cameraConstants.m_worldToCamera;
}



//----------------------------------------------------------------------------------------------------------------------
Mat44 Camera::GetOrthoProjectionMatrix() const
{
    UpdateProjMatrix();
    return m_cameraConstants.m_cameraToClip;
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::UpdateViewMatrix() const
{ 
    if (m_viewMatrixDirty)
    {
        Mat44 cameraToWorld = Mat44::CreateZRotationDegrees(m_rotation2D);
        cameraToWorld.SetTranslation3D(m_position);
        m_cameraConstants.m_worldToCamera = cameraToWorld.GetOrthoNormalInverse();
        m_viewMatrixDirty = false;
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::UpdateProjMatrix() const
{
    if (m_projMatrixDirty)
    {
        m_cameraConstants.m_cameraToClip = Mat44::CreateOrthoProjection(m_mins.x, m_maxs.x, m_mins.y, m_maxs.y, m_mins.z, m_maxs.z);
        m_projMatrixDirty = false;
    }
}
