﻿// Bradley Christensen - 2023
#pragma once



//----------------------------------------------------------------------------------------------------------------------
enum class CollisionType
{
    // todo: make collision layers etc, etc.
    Static,
    Mobile,
};



//----------------------------------------------------------------------------------------------------------------------
struct CCollision
{
    CCollision() = default;
    CCollision(void const* xmlElement);
    
    float m_radius = 0.f;
    CollisionType m_type = CollisionType::Static;
};
