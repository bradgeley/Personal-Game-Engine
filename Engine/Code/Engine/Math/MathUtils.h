// Bradley Christensen - 2022-2023
#pragma once



//----------------------------------------------------------------------------------------------------------------------
// MathUtils
//
// Common math operations
//


struct IntVec2;
struct Vec2;
struct Vec3;


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
float GetLength2D(const Vec2& vec);
float GetLengthSquared2D(const Vec2& vec);
void  NormalizeVector2D(Vec2& vec);
float DotProduct2D(const Vec2& vecA, const Vec2& vecB);


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
float GetFractionWithin(float val, float min, float max);
float RangeMap(float valueInRangeA, float minRangeA, float maxRangeA, float minRangeB, float maxRangeB);


//----------------------------------------------------------------------------------------------------------------------
// Simple Functions
//
int   IncrementIntInRange(int val, int rangeMin, int rangeMax, bool wrap = false);
int   DecrementIntInRange(int val, int rangeMin, int rangeMax, bool wrap = false);
float MinF(float a, float b);
float MaxF(float a, float b);
int   Min(int a, int b);
int   Max(int a, int b);
int   FloorF(float x);
int   CeilingF(float x);