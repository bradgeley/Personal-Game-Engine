// Bradley Christensen - 2022-2023
#include "MathUtils.h"
#include "Vec2.h"
#include "Vec3.h"

#define _USE_MATH_DEFINES
#include "cmath"


float SinDegrees(float degrees)
{
    float radians = DegreesToRadians(degrees);
    return sinf(radians);
}


float CosDegrees(float degrees)
{
    float radians = DegreesToRadians(degrees);
    return cosf(radians);
}


float SinRadians(float radians)
{
    return sinf(radians);
}


float CosRadians(float radians)
{
    return cosf(radians);
}


float DegreesToRadians(float degrees)
{
    return degrees * static_cast<float>(M_PI) / 180.f;
}


float RadiansToDegrees(float radians)
{
    return radians * 180.f / static_cast<float>(M_PI);
}


float ClampF(float value, float min, float max)
{
    if (value > max)
    {
        return max;
    }
    if (value < min)
    {
        return min;
    }
    return value;
}


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


float Interpolate(float a, float b, float t)
{
    return (b - a) * t + a;
}


float InterpolateClamped(float a, float b, float t)
{
    t = ClampF(t, 0.f, 1.f);
    return (b - a) * t + a;
}


int InterpolateInt(int a, int b, float t)
{
    return (int) Interpolate((float) a, (float) b, t);
}


int InterpolateIntClamped(int a, int b, float t)
{
    t = ClampF(t, 0.f, 1.f);
    return (int) Interpolate((float) a, (float) b, t);
}


float GetFractionWithin(float val, float min, float max)
{
    float range = max - min;
    float valRelative = val - min;
    return valRelative / range;
}


float RangeMap(float valueInRangeA, float minRangeA, float maxRangeA, float minRangeB, float maxRangeB)
{
    float fractionWithin = GetFractionWithin(valueInRangeA, minRangeA, maxRangeA);
    float rangeB = maxRangeB - minRangeB;
    return minRangeB + fractionWithin * rangeB;
}


float MinF(float a, float b)
{
	return a <= b ? a : b;
}


float MaxF(float a, float b)
{
	return a >= b ? a : b;
}
