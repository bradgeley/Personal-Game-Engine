// Bradley Christensen - 2022
#include "MathUtils.h"
#include "cmath"
#include "Vec2.h"



float GetLength2D(Vec2 const& vec)
{
    return sqrtf(vec.x * vec.x + vec.y * vec.y);
}



void NormalizeVector2D(Vec2& vec)
{
    float length = GetLength2D(vec);
    if (length != 0.f)
    {
        vec /= length;
    }
}



float DotProduct2D(Vec2 const& vecA, Vec2 const& vecB)
{
    return vecA.x * vecB.x + vecA.y * vecB.y;
}


