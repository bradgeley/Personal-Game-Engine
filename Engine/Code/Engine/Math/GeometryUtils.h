// Bradley Christensen - 2022-2023
#pragma once



struct Vec2;
struct AABB2;



//----------------------------------------------------------------------------------------------------------------------
// GeometryUtils
//
// Common geometric queries
//



//----------------------------------------------------------------------------------------------------------------------
// Line Intersection
//
bool DoLinesIntersect(Vec2 const& a1, Vec2 const& a2, Vec2 const& b1, Vec2 const& b2);
bool DoesLineIntersectAABB2(Vec2 const& lineStart, Vec2 const& lineEnd, AABB2 const& box2D);