// Bradley Christensen - 2023
#pragma once
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct CTransform
{
    CTransform() = default;
    
    Vec2 m_pos;
    float m_orientation = 0.f;
};
