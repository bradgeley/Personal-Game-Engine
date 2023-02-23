// Bradley Christensen - 2022
#include "MathUtils.h"
#include "cmath"
#include "Vec2.h"
#include "Vec3.h"


float GetLength2D(const Vec2& vec)
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


float DotProduct2D(const Vec2& vecA, const Vec2& vecB)
{
    return vecA.x * vecB.x + vecA.y * vecB.y;
}


float GetLength3D(const Vec3& vec) 
{
    return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}


void NormalizeVector3D(Vec3& vec)
{
    float length = GetLength3D(vec);
    if (length != 0.f)
    {
        vec /= length;
    }
}
