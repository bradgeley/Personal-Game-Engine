// Bradley Christensen - 2022-2025
#pragma once



//----------------------------------------------------------------------------------------------------------------------
struct CCollision
{
    CCollision() = default;
    CCollision(void const* xmlElement);
    
    float m_radius = 0.f;
};
