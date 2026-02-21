// Bradley Christensen - 2022-2025
#include "MathUtils.h"
#include "Vec2.h"
#include "Vec3.h"

#define _USE_MATH_DEFINES
#include <cmath>



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
double MathUtils::Clamp01(double value)
{
    return Clamp(value, 0.0, 1.0);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::Clamp01F(float value)
{
    return Clamp(value, 0.f, 1.f);
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
float MathUtils::NormalizeVector2D_ReturnLength(Vec2& vec)
{
    float length = GetLength2D(vec);
    if (!IsNearlyZero(length))
    {
        vec /= length;
    }
    return length;
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
void MathUtils::RotateVector2D(Vec2& vec, float radians)
{
    float cos = CosRadians(radians);
    float sin = SinRadians(radians);
    float x = vec.x * cos - vec.y * sin;
    float y = vec.x * sin + vec.y * cos;
    vec.x = x;
	vec.y = y;
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::GetLength3D(Vec3 const& vec)
{
    return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::GetDistance3D(Vec3 const& a, const Vec3& b)
{
    return MathUtils::GetLength3D(b - a);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::GetDistanceSquared3D(Vec3 const& a, const Vec3& b)
{
    return MathUtils::GetLengthSquared3D(b - a);
}



//----------------------------------------------------------------------------------------------------------------------
float MathUtils::GetLengthSquared3D(Vec3 const& vec)
{
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
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
    t = Clamp(t, 0.f, 1.f);
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
    t = Clamp(t, 0.f, 1.f);
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
    return Clamp(RangeMap(valueInRangeA, minRangeA, maxRangeA, minRangeB, maxRangeB), minRangeB, maxRangeB);
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::WrapInteger(int val, int rangeMin, int rangeMax)
{
    int range = rangeMax - rangeMin;
    if (range == 0)
    {
        return rangeMin;
    }

    if (range < 0)
    {
		Swap(rangeMin, rangeMax);
        range = -range;
    }

    int remainder = (val - rangeMin) % range;
    if (remainder < 0)
    {
        // Fell off the left end of the range, wrap back around to the top
        return rangeMax + remainder;
	}

    // Either is within the range, or
	// Fell off the right end of the range, wrap back around to the bottom
    return rangeMin + remainder;
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
    return static_cast<int>(floor(d));
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::CeilingF(float val)
{
    double d = (double) val;
    return static_cast<int>(ceil(d));
}



//----------------------------------------------------------------------------------------------------------------------
int MathUtils::RoundF(float val)
{
    return static_cast<int>(std::roundf(val));
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
bool MathUtils::IsNearlyEqual(float val, float comparison, float epsilon /*= 0.000001f*/)
{
    return MathUtils::AbsF(val - comparison) <= epsilon;
}



//----------------------------------------------------------------------------------------------------------------------
bool MathUtils::IsNearlyZero(float val, float epsilon /*= 0.000'001f*/)
{
    return MathUtils::AbsF(val) <= epsilon;
}
