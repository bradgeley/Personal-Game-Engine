// Bradley Christensen - 2022-2023
#pragma once


struct IntVec2;
struct Vec2;
struct Vec3;



//----------------------------------------------------------------------------------------------------------------------
// MathUtils
//
// Common math operations
//



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
int   ClampInt(int value, int min, int max);
float ClampF(float value, float min, float max);


//----------------------------------------------------------------------------------------------------------------------
// 2D Vector
//
float GetLength2D(Vec2 const& vec);
float GetLengthSquared2D(Vec2 const& vec);
void  NormalizeVector2D(Vec2& vec);
float DotProduct2D(Vec2 const& vecA, Vec2 const& vecB);
float GetDistance2D(Vec2 const& vecA, Vec2 const& vecB);
float GetDistanceSquared2D(Vec2 const& vecA, Vec2 const& vecB);


//----------------------------------------------------------------------------------------------------------------------
// 3D Vector
//
float GetLength3D(const Vec3& vec);
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
int   IncrementIntInRange(int val, int rangeMin, int rangeMax, bool wrap = false);
int   DecrementIntInRange(int val, int rangeMin, int rangeMax, bool wrap = false);
float MinF(float a, float b);
float MaxF(float a, float b);
int   Min(int a, int b);
int   Max(int a, int b);
int   Pow(int base, int exp);
float PowF(float base, float exp);
int   FloorF(float val);
int   CeilingF(float val);
float SqrtF(float val);
float AbsF(float val);
void  SwapF(float& a, float& b);
int	  Sign(int val);
float SignF(float val);