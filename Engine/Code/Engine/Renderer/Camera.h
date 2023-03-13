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
    
    void SetOrthoBounds(Vec3 const& mins, Vec3 const& maxs);

    AABB2 GetOrthoBounds2D() const;
    Mat44 GetOrthoProjectionMatrix() const;
    Vec3 GetOrthoDimensions() const;
    Vec2 ScreenToWorldOrtho(Vec2 const& relativeScreenPos) const;

private:
    
    Vec3 m_mins = Vec3::ZeroVector;
    Vec3 m_maxs = Vec3::ZeroVector;
};