// Bradley Christensen - 2022-2025
#pragma once


struct IntVec2;
struct Vec2;
struct Vec3;



//----------------------------------------------------------------------------------------------------------------------
// MathUtils
//
// Common math operations
//
namespace MathUtils
{
	//----------------------------------------------------------------------------------------------------------------------
	// Constants
	//
	constexpr float SQRT2F = 1.4142136f;
	constexpr float SQRT2OVER2F = SQRT2F / 2;
	constexpr double SQRT2 = 1.4142135623730951;



	//----------------------------------------------------------------------------------------------------------------------
	// Quadratic Equation
	//
	// Returns the number of real roots that were found and passed back out of the function (through out_root1 and out_root2)
	//
	int QuadraticEquation(float a, float b, float c, float& out_root1, float& out_root2);



	//----------------------------------------------------------------------------------------------------------------------
	// Trig
	//
	float SinDegrees(float degrees);
	float CosDegrees(float degrees);
	float SinRadians(float radians);
	float CosRadians(float radians);
	float DegreesToRadians(float degrees);
	float RadiansToDegrees(float radians);


	//----------------------------------------------------------------------------------------------------------------------
	// Utilities
	//
	double Clamp01(double value);
	float Clamp01F(float value);


	//----------------------------------------------------------------------------------------------------------------------
	// 2D Vector
	//
	float GetLength2D(Vec2 const& vec);
	float GetLengthSquared2D(Vec2 const& vec);
	void  NormalizeVector2D(Vec2& vec);
	float DotProduct2D(Vec2 const& vecA, Vec2 const& vecB);
	float CrossProduct2D(Vec2 const& vecA, Vec2 const& vecB);
	float GetDistance2D(Vec2 const& vecA, Vec2 const& vecB);
	float GetDistanceSquared2D(Vec2 const& vecA, Vec2 const& vecB);


	//----------------------------------------------------------------------------------------------------------------------
	// 3D Vector
	//
	float GetLength3D(Vec3 const& vec);
	float GetDistance3D(Vec3 const& a, const Vec3& b);
	float GetDistanceSquared3D(Vec3 const& a, const Vec3& b);
	float GetLengthSquared3D(Vec3 const& vec);
	void  NormalizeVector3D(Vec3& vec);


	//----------------------------------------------------------------------------------------------------------------------
	// Lerp
	//
	float Interpolate(float a, float b, float t);
	float InterpolateClamped(float a, float b, float t);
	int   InterpolateInt(int a, int b, float t);
	int   InterpolateIntClamped(int a, int b, float t);
	float SmoothStep3(float valZeroToOne);
	float SmoothStart2(float valZeroToOne);
	float SmoothStart3(float valZeroToOne);


	//----------------------------------------------------------------------------------------------------------------------
	// Range Map
	//
	float GetFractionWithinF(float val, float min, float max);
	double GetFractionWithin(double val, double min, double max);
	float RangeMap(float valueInRangeA, float minRangeA, float maxRangeA, float minRangeB, float maxRangeB);
	float RangeMapClamped(float valueInRangeA, float minRangeA, float maxRangeA, float minRangeB, float maxRangeB);


	//----------------------------------------------------------------------------------------------------------------------
	// Simple Functions
	//
	int		IncrementIntInRange(int val, int rangeMin, int rangeMax, bool wrap = false);
	int		DecrementIntInRange(int val, int rangeMin, int rangeMax, bool wrap = false);
	int		Pow(int base, int exp);
	float	PowF(float base, float exp);
	double	Log(double val);
	float	LogF(float val);
	int		FloorF(float val);
	int		CeilingF(float val);
	int		RoundF(float val);
	float	SqrtF(float val);
	int		Abs(int val);
	float	AbsF(float val);
	bool	IsNearlyEqual(float val, float comparison, float epsilon = 0.000001f);
	bool	IsNearlyZero(float val, float epsilon = 0.000001f);

	//----------------------------------------------------------------------------------------------------------------------
	// Simple Templated Functions
	//
	template <typename T>
	T Min(T const& a, T const& b);

	template <typename T>
	T Max(T const& a, T const& b);

	template <typename T>
	T Sign(T const& value);

	template <typename T>
	T Abs(T const& value);

	template <typename T>
	void Swap(T& a, T& b);

	template <typename T>
	T Clamp(T const& val, T const& rangeMin, T const& rangeMax);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T MathUtils::Min(T const& a, T const& b)
{
	if (a <= b) 
	{
		return a;
	}
	else 
	{
		return b;
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T MathUtils::Max(T const& a, T const& b)
{
	if (a >= b)
	{
		return a;
	}
	else
	{
		return b;
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T MathUtils::Sign(T const& value)
{
	if (value > T(0)) 
	{
		return T(1);
	}
	if (value < T(0))
	{
		return T(-1);
	}
	return T(0);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T MathUtils::Abs(T const& value)
{
	if (value < T(0))
	{
		return -value;
	}
	else
	{
		return value;
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T MathUtils::Clamp(T const& val, T const& rangeMin, T const& rangeMax)
{
	if (val < rangeMin) 
	{
		return rangeMin;
	}
	if (val > rangeMax) 
	{
		return rangeMax;
	}
	return val;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void MathUtils::Swap(T& a, T& b)
{
	T intermediate = a;
	a = b;
	b = intermediate;
}