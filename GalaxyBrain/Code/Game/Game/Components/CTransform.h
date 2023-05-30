// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct CTransform : Component
{
    Component* DeepCopy() const override;
    
    Vec2 m_pos;
    float m_orientation = 0.f;
};
