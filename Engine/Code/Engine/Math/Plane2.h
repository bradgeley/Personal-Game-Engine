// Bradley Christensen - 2022-2025
#pragma once
#include "Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
// Plane 2D
//
struct Plane2
{
public:

    Vec2 m_normal;
    float m_distance;

public:
    
    explicit Plane2(Vec2 const& normal, float distance = 0.f);
    explicit Plane2(Vec2 const& from, Vec2 const& to); // normal will face to the right going from -> to

public:
    
    float GetAltitude(Vec2 const& point) const;
    float GetDistance(Vec2 const& point) const;
    bool Straddles(Vec2 const& a, Vec2 const& b) const;
};