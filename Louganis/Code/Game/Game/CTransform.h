// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/EntityID.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct CTransform
{
    CTransform() = default;
    
    Vec2 m_pos = Vec2::ZeroVector;
    float m_orientation = 0.f;
};
