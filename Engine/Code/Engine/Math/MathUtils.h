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
float ClampF(float value, float min, float max);


//----------------------------------------------------------------------------------------------------------------------
// 2D Vector
//
float GetLength2D(const Vec2& vec);
void NormalizeVector2D(Vec2& vec);
float DotProduct2D(const Vec2& vecA, const Vec2& vecB);


//----------------------------------------------------------------------------------------------------------------------
// 3D Vector
//
float GetLength3D(const Vec3& vec);
void NormalizeVector3D(Vec3& vec);


//----------------------------------------------------------------------------------------------------------------------
// Lerp
//
float Interpolate(float a, float b, float t);
float InterpolateClamped(float a, float b, float t);
int InterpolateInt(int a, int b, float t);
int InterpolateIntClamped(int a, int b, float t);
