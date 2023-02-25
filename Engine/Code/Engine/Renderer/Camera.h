// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Math/Mat44.h"
#include "Engine/Math/Vec3.h"



class Camera
{
public:

    Camera() = default;
	explicit Camera(Vec3 const& bottomLeft, Vec3 const& topRight);
    
    Mat44 GetOrthoProjectionMatrix() const;

private:
    
    Vec3 m_mins = Vec3::ZeroVector;
    Vec3 m_maxs = Vec3::ZeroVector;
};