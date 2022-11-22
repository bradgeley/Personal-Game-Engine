// Bradley Christensen - 2022
#pragma once


//----------------------------------------------------------------------------------------------------------------------
// MathUtils
//
// Common math operations
//


struct Vec2;


//----------------------------------------------------------------------------------------------------------------------
// 2D Vector
//
float GetLength2D(const Vec2& vec);
void NormalizeVector2D(Vec2& vec);
float DotProduct2D(const Vec2& vecA, const Vec2& vecB);
