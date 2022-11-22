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
float GetLength2D(Vec2 const& vec);
void NormalizeVector2D(Vec2& vec);
float DotProduct2D(Vec2 const& vecA, Vec2 const& vecB);

