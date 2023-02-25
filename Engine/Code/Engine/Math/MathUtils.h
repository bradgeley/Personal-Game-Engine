// Bradley Christensen - 2022
#pragma once


//----------------------------------------------------------------------------------------------------------------------
// MathUtils
//
// Common math operations
//


struct Vec2;
struct Vec3;

//----------------------------------------------------------------------------------------------------------------------
// Math
//
float SinDegrees(float degrees);
float CosDegrees(float degrees);
float SinRadians(float radians);
float CosRadians(float radians);
float DegreesToRadians(float degrees);
float RadiansToDegrees(float radians);


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
int InterpolateInt(int a, int b, float t);
