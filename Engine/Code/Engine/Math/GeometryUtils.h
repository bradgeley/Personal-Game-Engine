// Bradley Christensen - 2022-2026
#pragma once



struct Vec2;
struct AABB2;



//----------------------------------------------------------------------------------------------------------------------
// GeometryUtils
//
// Common geometric queries and utility functions
//
namespace GeometryUtils
{
	bool IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float discRadius);



	//----------------------------------------------------------------------------------------------------------------------
	// 2D Overlap Test
	//
	bool DoDiscsOverlap2D(Vec2 const& position1, float radius1, Vec2 const& position2, float radius2);
	bool DoesDiscOverlapAABB(Vec2 const& discPos, float discRadius, AABB2 const& aabb);
	bool DoesRingOverlapAABB(Vec2 const& ringPos, float innerRadius, float outerRadius, AABB2 const& aabb);
	bool DoesCapsuleOverlapAABB(Vec2 const& capsuleStart, Vec2 const& capsuleEnd, float capsuleRadius, AABB2 const& aabb);



	//----------------------------------------------------------------------------------------------------------------------
	// 2D Collision Resolution
	//
	bool PushDiscOutOfPoint2D(Vec2& discPos, float radius, Vec2 const& point);
	bool PushDiscOutOfDisc2D(Vec2& mobileDiscPos, float mobileDiscRadius, Vec2 const& staticDiscPos, float staticDiscRadius);
	bool PushDiscsOutOfEachOther2D(Vec2& discPosA, float discRadiusA, Vec2& discPosB, float discRadiusB, bool weightByRadius = true, float dampening = 1.f);
	bool PushDiscOutOfAABB2D(Vec2& discPos, float discRadius, AABB2 const& aabb);



	//----------------------------------------------------------------------------------------------------------------------
	// 2D Bounds
	//
	AABB2 GetDiscBounds(Vec2 const& center, float radius);
	AABB2 GetCapsuleBounds(Vec2 const& start, Vec2 const& end, float radius);



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
	bool GetFirstLineAABBIntersection(Vec2 const& lineStart, Vec2 const& lineEnd, AABB2 const& box2D, float& out_tOfFirstIntersection);



	//----------------------------------------------------------------------------------------------------------------------
	// Nearest Point
	//
	Vec2 GetNearestPointOnLine(Vec2 const& queryPoint, Vec2 const& lineStart, Vec2 const& lineEnd);



	//----------------------------------------------------------------------------------------------------------------------
	// 2D Shortest Distance
	//
	float GetShortestDistanceToLineSegment(Vec2 const& queryPoint, Vec2 const& lineStart, Vec2 const& lineEnd);
	float GetShortestDistanceSquaredToLineSegment(Vec2 const& queryPoint, Vec2 const& lineStart, Vec2 const& lineEnd);

	float GetShortestDistanceBetweenLineSegments(Vec2 const& aStart, Vec2 const& aEnd, Vec2 const& bStart, Vec2 const& bEnd);
	float GetShortestDistanceSquaredBetweenLineSegments(Vec2 const& aStart, Vec2 const& aEnd, Vec2 const& bStart, Vec2 const& bEnd);

	float GetShortestDistanceBetweenLineSegmentAndAABB(Vec2 const& lineStart, Vec2 const& lineEnd, AABB2 const& aabb);
	float GetShortestDistanceSquaredBetweenLineSegmentAndAABB(Vec2 const& lineStart, Vec2 const& lineEnd, AABB2 const& aabb);



	//----------------------------------------------------------------------------------------------------------------------
	// Disc Sweep
	//
	bool SweepDiscVsPoint(Vec2 const& discStart, Vec2 const& discEnd, float discRadius, Vec2 const& point, float& out_tOfIntersection);
}
