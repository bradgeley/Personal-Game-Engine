// Bradley Christensen - 2022-2023
#include "MathUtils.h"
#include "Vec2.h"
#include "Vec3.h"

#define _USE_MATH_DEFINES
#include "cmath"


int QuadraticEquation(float a, float b, float c, float& out_root1, float& out_root2)
{
    float descriminant = b * b - 4 * a * c;
    if (descriminant < 0.f)
    {
        return 0;
    }

    if (descriminant == 0.f)
    {
        out_root1 = -b / (2.f * a);
        return 1;
    }
    else
    {
        float sqrtDesc = SqrtF(descriminant);
        float oneOverTwoA = 1.f / (2.f * a);
        out_root1 = (-b + sqrtDesc) * oneOverTwoA;
        out_root2 = (-b - sqrtDesc) * oneOverTwoA;
        return 2;
    }
}


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


int ClampInt(int value, int min, int max)
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


float GetLengthSquared2D(const Vec2& vec)
{
    return vec.x * vec.x + vec.y * vec.y;
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


float GetDistance2D(Vec2 const& vecA, Vec2 const& vecB)
{
    Vec2 aToB = vecA - vecB;
    return aToB.GetLength();
}


float GetDistanceSquared2D(Vec2 const& vecA, Vec2 const& vecB)
{
    Vec2 aToB = vecA - vecB;
    return aToB.GetLengthSquared();
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


float SmoothStep3(float valZeroToOne)
{
    // -2*x^3 + 3*x^2
    float valSquared = valZeroToOne * valZeroToOne;
    return -2 * valSquared * valZeroToOne + 3 * valSquared;
}


float SmoothStart2(float valZeroToOne)
{
    return valZeroToOne * valZeroToOne;
}


float SmoothStart3(float valZeroToOne)
{
    return valZeroToOne * valZeroToOne * valZeroToOne;
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


int IncrementIntInRange(int val, int rangeMin, int rangeMax, bool wrap)
{
    ++val;
    if (val > rangeMax)
    {
        if (wrap)
        {
            val = rangeMin;
        }
        else
        {
            val = rangeMax;
        }
    }
    return val;
}


int DecrementIntInRange(int val, int rangeMin, int rangeMax, bool wrap)
{
    --val;
    if (val < rangeMin)
    {
        if (wrap)
        {
            val = rangeMax;
        }
        else
        {
            val = rangeMin;
        }
    }
    return val;
}


float MinF(float a, float b)
{
	return a <= b ? a : b;
}


float MaxF(float a, float b)
{
	return a >= b ? a : b;
}


int Min(int a, int b)
{
	return a <= b ? a : b;
}


int Max(int a, int b)
{
	return a >= b ? a : b;
}


int FloorF(float val)
{
    double d = (double) val;
    return (int) floor(d);
}


int CeilingF(float val)
{
    double d = (double) val;
    return (int) ceil(d);
}


float SqrtF(float val)
{
    return sqrtf(val);
}


float AbsF(float val)
{
    return fabsf(val);
}
