// Bradley Christensen - 2022-2023
#pragma once
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
    
    void SetOrthoDims(Vec3 const& dims);
    void SetOrthoDims2D(Vec2 const& dims2D);
    void SetOrthoBounds(Vec3 const& mins, Vec3 const& maxs);
    void SetOrthoCenter(Vec3 const& center);
    void SetOrthoCenter2D(Vec2 const& center);

    Vec3  GetOrthoCenter() const;
    Vec2  GetOrthoCenter2D() const;
    AABB2 GetOrthoBounds2D() const;
    Mat44 GetOrthoProjectionMatrix() const;
    Vec3 GetOrthoDimensions() const;
    Vec3 GetOrthoHalfDimensions() const;
    Vec2 ScreenToWorldOrtho(Vec2 const& relativeScreenPos) const;

private:
    
    Vec3 m_mins = Vec3::ZeroVector;
    Vec3 m_maxs = Vec3(1.f, 1.f, 1.f);
};
