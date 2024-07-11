// Bradley Christensen - 2022-2024
#pragma once
#include "Engine/Renderer/EngineConstantBuffers.h"
#include "Engine/Math/Mat44.h"
#include "Engine/Math/Vec3.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Math/AABB2.h"



//----------------------------------------------------------------------------------------------------------------------
class Camera
{
public:

    Camera() = default;
	explicit Camera(Vec3 const& bottomLeft, Vec3 const& topRight);
    explicit Camera(float minX, float minY, float maxX, float maxY);
    explicit Camera(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

    void DefineGameSpace(Vec3 const& gameForward, Vec3 const& gameLeft, Vec3 const& gameUp);

    void SetPosition(Vec3 const& position);
    void Translate(Vec3 const& deltaPos);
    void Translate2D(Vec2 const& deltaPos);
    void SetRotation2D(float rotation);
    void SetOrthoDims(Vec3 const& dims);
    void SetOrthoDims2D(Vec2 const& dims2D);
    void SetOrthoBounds(Vec3 const& mins, Vec3 const& maxs);
    void SetOrthoBounds2D(AABB2 const& orthoBounds);
    void SetOrthoCenter(Vec3 const& center);
    void SetOrthoCenter2D(Vec2 const& center);
    void ZoomAroundCenter2D(float zoomRatio, Vec2 const& center);

    void SetCameraConstants(CameraConstants const& cc);

    Vec3 const& GetPosition() const;
    float GetRotation2D() const;
    Vec3 GetOrthoCenter() const;
    Vec2 GetOrthoCenter2D() const;
    AABB2 GetOrthoBounds2D() const;
    Vec3 GetOrthoDimensions() const;
    Vec3 GetOrthoHalfDimensions() const;
    Vec2 ScreenToWorldOrtho(Vec2 const& relativeScreenPos) const;

    // Const Warning: These functions may recalculate the view and projection matrices
    CameraConstants const& GetCameraConstants() const;
    Mat44 GetViewMatrix() const;
    Mat44 GetOrthoProjectionMatrix() const;

private:

    // Const Warning: Const because called in Renderer::BeginCamera, but they update m_cameraConstants
    void UpdateViewMatrix() const;
    void UpdateProjMatrix() const;

private:
    
    // Const Warning: Mutable because it needs to be updated if dirty in BeginCamera
    mutable bool m_viewMatrixDirty = true;
    mutable bool m_projMatrixDirty = true;
    mutable CameraConstants m_cameraConstants;

    Vec3 m_mins             = Vec3::ZeroVector;
    Vec3 m_maxs             = Vec3(1.f, 1.f, 1.f);
    Vec3 m_position         = Vec3::ZeroVector;
    float m_rotation2D      = 0.f; // todo: Euler Angles or Quat
};
