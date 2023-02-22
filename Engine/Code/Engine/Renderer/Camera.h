// Bradley Christensen - 2023
#pragma once
#include "Engine/Math/Vec3.h"



class Camera
{
public:
    
	explicit Camera(Vec3 const& bottomLeft, Vec3 const& topRight);

private:
    
    Vec3 m_mins = Vec3::ZeroVector;
    Vec3 m_maxs = Vec3::ZeroVector;
};