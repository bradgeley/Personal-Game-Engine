// Bradley Christensen - 2022-2023
#pragma once
#include "Rgba8.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Math/Vec3.h"



struct Vertex_PCU
{
public:
    
    Vec3    pos;
    Rgba8   tint;
    Vec2    uvs;

public:

    explicit Vertex_PCU(Vec3 const& position, Rgba8 const& tint = Rgba8::WHITE, Vec2 const& uvs = Vec2::ZeroToOne);
};
