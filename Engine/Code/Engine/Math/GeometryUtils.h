// Bradley Christensen - 2022-2023
#pragma once



struct Vec2;
struct AABB2;



//----------------------------------------------------------------------------------------------------------------------
// GeometryUtils
//
// Common geometric queries and utility functions
//
bool IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float discRadius);



//----------------------------------------------------------------------------------------------------------------------
// 2D Collision
//
bool DoDiscsOverlap2D(Vec2 const& position1, float radius1, Vec2 const& position2, float radius2);
bool PushDiscOutOfPoint2D(Vec2& discPos, float radius, Vec2 const& point);
bool PushDiscOutOfDisc2D(Vec2& mobileDiscPos, float mobileDiscRadius, Vec2 const& staticDiscPos, float staticDiscRadius);
bool PushDiscsOutOfEachOther2D(Vec2& discPosA, float discRadiusA, Vec2& discPosB, float discRadiusB);
bool PushDiscOutOfAABB2D(Vec2& discPos, float discRadius, AABB2 const& aabb);



//----------------------------------------------------------------------------------------------------------------------
// 2D Physics
//
bool BounceDiscsOffEachOther2D(Vec2& discPosA, float discRadiusA, Vec2& discVelA, Vec2& discPosB, float discRadiusB, Vec2& discVelB, float elasticity);
bool BounceDiscsOffEachOther2D(Vec2& discPosA, float discRadiusA, Vec2& discVelA, float massA, Vec2& discPosB, float discRadiusB, Vec2& discVelB, float massB, float elasticity = 1.f);



//----------------------------------------------------------------------------------------------------------------------
// Line Intersection
//
bool DoLinesIntersect(Vec2 const& a1, Vec2 const& a2, Vec2 const& b1, Vec2 const& b2);
bool DoesLineIntersectAABB2(Vec2 const& lineStart, Vec2 const& lineEnd, AABB2 const& box2D);