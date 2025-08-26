// Bradley Christensen - 2022-2025
#include "MathUtils.h"
#include "Vec2.h"
#include "Vec3.h"

#define _USE_MATH_DEFINES
#include "cmath"



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::QuadraticEquation(float a, float b, float c, float& out_root1, float& out_root2)
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



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::SinDegrees(float degrees)
{
    float radians = DegreesToRadians(degrees);
    return sinf(radians);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::CosDegrees(float degrees)
{
    float radians = DegreesToRadians(degrees);
    return cosf(radians);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::SinRadians(float radians)
{
    return sinf(radians);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::CosRadians(float radians)
{
    return cosf(radians);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::DegreesToRadians(float degrees)
{
    return degrees * static_cast<float>(M_PI) / 180.f;
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::RadiansToDegrees(float radians)
{
    return radians * 180.f / static_cast<float>(M_PI);
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::ClampInt(int value, int min, int max)
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



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::ClampF(float value, float min, float max)
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



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::Clamp01F(float value)
{
    return ClampF(value, 0.f, 1.f);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::GetLength2D(const Vec2& vec)
{
    return sqrtf(vec.x * vec.x + vec.y * vec.y);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::GetLengthSquared2D(const Vec2& vec)
{
    return vec.x * vec.x + vec.y * vec.y;
}



//----------------------------------------------------------------------------------------------------------------------
void MathUtils::NormalizeVector2D(Vec2& vec)
{
    float length = GetLength2D(vec);
    if (!IsNearlyZero(length))
    {
        vec /= length;
    }
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::DotProduct2D(const Vec2& vecA, const Vec2& vecB)
{
    return vecA.x * vecB.x + vecA.y * vecB.y;
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::CrossProduct2D(Vec2 const& vecA, Vec2 const& vecB)
{
    return vecA.x * vecB.y - vecA.y * vecB.x;
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::GetDistance2D(Vec2 const& vecA, Vec2 const& vecB)
{
    Vec2 aToB = vecA - vecB;
    return aToB.GetLength();
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::GetDistanceSquared2D(Vec2 const& vecA, Vec2 const& vecB)
{
    Vec2 aToB = vecA - vecB;
    return aToB.GetLengthSquared();
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::GetLength3D(const Vec3& vec)
{
    return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}



//----------------------------------------------------------------------------------------------------------------------
void MathUtils::NormalizeVector3D(Vec3& vec)
{
    float length = GetLength3D(vec);
    if (length != 0.f)
    {
        vec /= length;
    }
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::Interpolate(float a, float b, float t)
{
    return (b - a) * t + a;
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::InterpolateClamped(float a, float b, float t)
{
    t = ClampF(t, 0.f, 1.f);
    return (b - a) * t + a;
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::InterpolateInt(int a, int b, float t)
{
    return (int) Interpolate((float) a, (float) b, t);
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::InterpolateIntClamped(int a, int b, float t)
{
    t = ClampF(t, 0.f, 1.f);
    return (int) Interpolate((float) a, (float) b, t);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::SmoothStep3(float valZeroToOne)
{
    // -2*x^3 + 3*x^2
    float valSquared = valZeroToOne * valZeroToOne;
    return -2 * valSquared * valZeroToOne + 3 * valSquared;
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::SmoothStart2(float valZeroToOne)
{
    return valZeroToOne * valZeroToOne;
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::SmoothStart3(float valZeroToOne)
{
    return valZeroToOne * valZeroToOne * valZeroToOne;
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::GetFractionWithinF(float val, float min, float max)
{
    float range = max - min;
    float valRelative = val - min;
    return valRelative / range;
}




//----------------------------------------------------------------------------------------------------------------------
double MathUtils::GetFractionWithin(double val, double min, double max)
{
    double range = max - min;
    double valRelative = val - min;
    return valRelative / range;
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::RangeMap(float valueInRangeA, float minRangeA, float maxRangeA, float minRangeB, float maxRangeB)
{
    float fractionWithin = GetFractionWithinF(valueInRangeA, minRangeA, maxRangeA);
    float rangeB = maxRangeB - minRangeB;
    return minRangeB + fractionWithin * rangeB;
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::RangeMapClamped(float valueInRangeA, float minRangeA, float maxRangeA, float minRangeB, float maxRangeB)
{
    return ClampF(RangeMap(valueInRangeA, minRangeA, maxRangeA, minRangeB, maxRangeB), minRangeB, maxRangeB);
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::IncrementIntInRange(int val, int rangeMin, int rangeMax, bool wrap)
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



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::DecrementIntInRange(int val, int rangeMin, int rangeMax, bool wrap)
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



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::MinF(float a, float b)
{
	return a <= b ? a : b;
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::MaxF(float a, float b)
{
	return a >= b ? a : b;
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::Min(int a, int b)
{
	return a <= b ? a : b;
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::Max(int a, int b)
{
	return a >= b ? a : b;
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::Pow(int base, int exp)
{
    return static_cast<int>(pow(base, exp));
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::PowF(float base, float exp)
{
    return powf(base, exp);
}



//----------------------------------------------------------------------------------------------------------------------
double MathUtils::Log(double val)
{
    return log(val);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::LogF(float val)
{
    return logf(val);
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::FloorF(float val)
{
    double d = (double) val;
    return (int) floor(d);
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::CeilingF(float val)
{
    double d = (double) val;
    return (int) ceil(d);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::SqrtF(float val)
{
    return sqrtf(val);
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::Abs(int val)
{
    return abs(val);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::AbsF(float val)
{
    return fabsf(val);
}



//----------------------------------------------------------------------------------------------------------------------
void MathUtils::SwapF(float& a, float& b)
{
    float a_copy = a;
    a = b;
    b = a_copy;
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::Sign(int val)
{
    if (val >= 0)
    {
        return 1;
    }
    return -1;
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::SignF(float val)
{
    if (val >= 0.f)
    {
        return 1.f;
    }
    return -1.f;
}



//----------------------------------------------------------------------------------------------------------------------
bool MathUtils::IsNearlyEqual(float val, float comparison, float epsilon /*= 0.000001f*/)
{
    return MathUtils::AbsF(val - comparison) <= epsilon;
}



//----------------------------------------------------------------------------------------------------------------------
bool MathUtils::IsNearlyZero(float val, float epsilon /*= 0.000001f*/)
{
    return MathUtils::AbsF(val) <= epsilon;
}
