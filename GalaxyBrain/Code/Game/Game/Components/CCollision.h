// Bradley Christensen - 2023
#pragma once



//----------------------------------------------------------------------------------------------------------------------
struct CCollision
{
    CCollision() = default;
    CCollision(void const* xmlElement);
    
    float m_radius = 0.f;
};
