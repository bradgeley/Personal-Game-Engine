// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Math/AABB2.h"



//----------------------------------------------------------------------------------------------------------------------
// GeometryUtils Unit Tests
//
namespace TestGeometryUtils
{

    //----------------------------------------------------------------------------------------------------------------------
    // IsPointInsideDisc2D
    //
    TEST(GeometryUtils, IsPointInsideDisc2D)
    {
        Vec2 center(0.0f, 0.0f);
        float radius = 5.0f;
        EXPECT_TRUE(GeometryUtils::IsPointInsideDisc2D(Vec2(0.0f, 0.0f), center, radius));
        EXPECT_TRUE(GeometryUtils::IsPointInsideDisc2D(Vec2(3.0f, 4.0f), center, radius));
        EXPECT_FALSE(GeometryUtils::IsPointInsideDisc2D(Vec2(6.0f, 0.0f), center, radius));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // DoDiscsOverlap2D
    //
    TEST(GeometryUtils, DoDiscsOverlap2D)
    {
        Vec2 a(0.0f, 0.0f);
        float ra = 2.0f;
        Vec2 b(3.0f, 0.0f);
        float rb = 2.0f;
        EXPECT_TRUE(GeometryUtils::DoDiscsOverlap2D(a, ra, b, rb));
        EXPECT_FALSE(GeometryUtils::DoDiscsOverlap2D(a, ra, Vec2(5.0f, 0.0f), rb));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // IsDiscTouchingAABB
    //
    TEST(GeometryUtils, IsDiscTouchingAABB)
    {
        Vec2 discPos(2.0f, 2.0f);
        float discRadius = 1.0f;
        AABB2 aabb(Vec2(0.0f, 0.0f), Vec2(3.0f, 3.0f));
        EXPECT_TRUE(GeometryUtils::IsDiscTouchingAABB(discPos, discRadius, aabb));
        EXPECT_FALSE(GeometryUtils::IsDiscTouchingAABB(Vec2(5.0f, 5.0f), discRadius, aabb));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // DoesCapsuleOverlapAABB
    //
    TEST(GeometryUtils, DoesCapsuleOverlapAABB)
    {
        Vec2 start(0.0f, 0.0f);
        Vec2 end(4.0f, 0.0f);
        float radius = 1.0f;
        AABB2 aabb(Vec2(2.0f, -1.0f), Vec2(3.0f, 1.0f));
        EXPECT_TRUE(GeometryUtils::DoesCapsuleOverlapAABB(start, end, radius, aabb));
        EXPECT_FALSE(GeometryUtils::DoesCapsuleOverlapAABB(Vec2(10.0f, 10.0f), Vec2(12.0f, 10.0f), radius, aabb));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // PushDiscOutOfPoint2D
    //
    TEST(GeometryUtils, PushDiscOutOfPoint2D)
    {
        Vec2 discPos(1.0f, 0.0f);
        float radius = 2.0f;
        Vec2 point(0.0f, 0.0f);
        EXPECT_TRUE(GeometryUtils::PushDiscOutOfPoint2D(discPos, radius, point));
        EXPECT_NEAR(discPos.GetLength(), radius, 1e-6f);

        Vec2 discPos2(3.0f, 0.0f);
        EXPECT_FALSE(GeometryUtils::PushDiscOutOfPoint2D(discPos2, radius, point));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // PushDiscOutOfDisc2D
    //
    TEST(GeometryUtils, PushDiscOutOfDisc2D)
    {
        Vec2 mobile(1.0f, 0.0f);
        float r1 = 1.0f;
        Vec2 stat(0.0f, 0.0f);
        float r2 = 1.0f;
        EXPECT_TRUE(GeometryUtils::PushDiscOutOfDisc2D(mobile, r1, stat, r2));
        EXPECT_NEAR(mobile.GetLength(), r1 + r2, 1e-6f);

        Vec2 mobile2(3.0f, 0.0f);
        EXPECT_FALSE(GeometryUtils::PushDiscOutOfDisc2D(mobile2, r1, stat, r2));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // PushDiscsOutOfEachOther2D
    //
    TEST(GeometryUtils, PushDiscsOutOfEachOther2D)
    {
        Vec2 a(0.0f, 0.0f);
        float ra = 1.0f;
        Vec2 b(1.0f, 0.0f);
        float rb = 1.0f;
        EXPECT_TRUE(GeometryUtils::PushDiscsOutOfEachOther2D(a, ra, b, rb));
        EXPECT_NEAR(a.GetDistanceTo(b), ra + rb, 1e-6f);

        Vec2 c(5.0f, 0.0f);
        Vec2 d(10.0f, 0.0f);
        EXPECT_FALSE(GeometryUtils::PushDiscsOutOfEachOther2D(c, ra, d, rb));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // PushDiscOutOfAABB2D
    //
    TEST(GeometryUtils, PushDiscOutOfAABB2D)
    {
        Vec2 discPos(1.0f, 1.0f);
        float radius = 1.0f;
        AABB2 aabb(Vec2(0.0f, 0.0f), Vec2(2.0f, 2.0f));
        EXPECT_TRUE(GeometryUtils::PushDiscOutOfAABB2D(discPos, radius, aabb));

        Vec2 discPos2(5.0f, 5.0f);
        EXPECT_FALSE(GeometryUtils::PushDiscOutOfAABB2D(discPos2, radius, aabb));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetDiscBounds
    //
    TEST(GeometryUtils, GetDiscBounds)
    {
        Vec2 center(2.0f, 3.0f);
        float radius = 1.5f;
        AABB2 bounds = GeometryUtils::GetDiscBounds(center, radius);
        EXPECT_EQ(bounds.mins, Vec2(0.5f, 1.5f));
        EXPECT_EQ(bounds.maxs, Vec2(3.5f, 4.5f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetCapsuleBounds
    //
    TEST(GeometryUtils, GetCapsuleBounds)
    {
        Vec2 start(1.0f, 2.0f);
        Vec2 end(4.0f, 6.0f);
        float radius = 1.0f;
        AABB2 bounds = GeometryUtils::GetCapsuleBounds(start, end, radius);
        EXPECT_EQ(bounds.mins, Vec2(0.0f, 1.0f));
        EXPECT_EQ(bounds.maxs, Vec2(5.0f, 7.0f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // DoLinesIntersect
    //
    TEST(GeometryUtils, DoLinesIntersect)
    {
        Vec2 a1(0.0f, 0.0f), a2(2.0f, 2.0f);
        Vec2 b1(0.0f, 2.0f), b2(2.0f, 0.0f);
        EXPECT_TRUE(GeometryUtils::DoLinesIntersect(a1, a2, b1, b2));

        Vec2 c1(0.0f, 0.0f), c2(1.0f, 0.0f);
        Vec2 d1(2.0f, 0.0f), d2(3.0f, 0.0f);
        EXPECT_FALSE(GeometryUtils::DoLinesIntersect(c1, c2, d1, d2));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // DoesLineIntersectAABB2
    //
    TEST(GeometryUtils, DoesLineIntersectAABB2)
    {
        Vec2 lineStart(0.0f, 1.0f), lineEnd(3.0f, 1.0f);
        AABB2 aabb(Vec2(1.0f, 0.0f), Vec2(2.0f, 2.0f));
        EXPECT_TRUE(GeometryUtils::DoesLineIntersectAABB2(lineStart, lineEnd, aabb));

        Vec2 lineStart2(0.0f, 3.0f), lineEnd2(3.0f, 3.0f);
        EXPECT_FALSE(GeometryUtils::DoesLineIntersectAABB2(lineStart2, lineEnd2, aabb));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetFirstLineAABBIntersection
    //
    TEST(GeometryUtils, GetFirstLineAABBIntersection)
    {
        Vec2 lineStart(0.0f, 1.0f), lineEnd(3.0f, 1.0f);
        AABB2 aabb(Vec2(1.0f, 0.0f), Vec2(2.0f, 2.0f));
        float t = -1.0f;
        EXPECT_TRUE(GeometryUtils::GetFirstLineAABBIntersection(lineStart, lineEnd, aabb, t));
        EXPECT_GE(t, 0.0f);
        EXPECT_LE(t, 1.0f);

        Vec2 lineStart2(0.0f, 3.0f), lineEnd2(3.0f, 3.0f);
        float t2 = -1.0f;
        EXPECT_FALSE(GeometryUtils::GetFirstLineAABBIntersection(lineStart2, lineEnd2, aabb, t2));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetNearestPointOnLine
    //
    TEST(GeometryUtils, GetNearestPointOnLine)
    {
        Vec2 query(1.0f, 2.0f);
        Vec2 start(0.0f, 0.0f);
        Vec2 end(2.0f, 0.0f);
        Vec2 nearest = GeometryUtils::GetNearestPointOnLine(query, start, end);
        EXPECT_NEAR(nearest.x, 1.0f, 1e-6f);
        EXPECT_NEAR(nearest.y, 0.0f, 1e-6f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetShortestDistanceToLineSegment
    //
    TEST(GeometryUtils, GetShortestDistanceToLineSegment)
    {
        Vec2 query(1.0f, 2.0f);
        Vec2 start(0.0f, 0.0f);
        Vec2 end(2.0f, 0.0f);
        float dist = GeometryUtils::GetShortestDistanceToLineSegment(query, start, end);
        EXPECT_NEAR(dist, 2.0f, 1e-6f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetShortestDistanceSquaredToLineSegment
    //
    TEST(GeometryUtils, GetShortestDistanceSquaredToLineSegment)
    {
        Vec2 query(1.0f, 2.0f);
        Vec2 start(0.0f, 0.0f);
        Vec2 end(2.0f, 0.0f);
        float distSq = GeometryUtils::GetShortestDistanceSquaredToLineSegment(query, start, end);
        EXPECT_NEAR(distSq, 4.0f, 1e-6f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetShortestDistanceBetweenLineSegments
    //
    TEST(GeometryUtils, GetShortestDistanceBetweenLineSegments)
    {
        Vec2 a1(0.0f, 0.0f), a2(1.0f, 0.0f);
        Vec2 b1(0.0f, 2.0f), b2(1.0f, 2.0f);
        float dist = GeometryUtils::GetShortestDistanceBetweenLineSegments(a1, a2, b1, b2);
        EXPECT_NEAR(dist, 2.0f, 1e-6f);

        // Intersecting
        Vec2 c1(0.0f, 0.0f), c2(1.0f, 1.0f);
        Vec2 d1(0.0f, 1.0f), d2(1.0f, 0.0f);
        float dist2 = GeometryUtils::GetShortestDistanceBetweenLineSegments(c1, c2, d1, d2);
        EXPECT_NEAR(dist2, 0.0f, 1e-6f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetShortestDistanceSquaredBetweenLineSegments
    //
    TEST(GeometryUtils, GetShortestDistanceSquaredBetweenLineSegments)
    {
        Vec2 a1(0.0f, 0.0f), a2(1.0f, 0.0f);
        Vec2 b1(0.0f, 2.0f), b2(1.0f, 2.0f);
        float distSq = GeometryUtils::GetShortestDistanceSquaredBetweenLineSegments(a1, a2, b1, b2);
        EXPECT_NEAR(distSq, 4.0f, 1e-6f);

        // Intersecting
        Vec2 c1(0.0f, 0.0f), c2(1.0f, 1.0f);
        Vec2 d1(0.0f, 1.0f), d2(1.0f, 0.0f);
        float distSq2 = GeometryUtils::GetShortestDistanceSquaredBetweenLineSegments(c1, c2, d1, d2);
        EXPECT_NEAR(distSq2, 0.0f, 1e-6f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetShortestDistanceBetweenLineSegmentAndAABB
    //
    TEST(GeometryUtils, GetShortestDistanceBetweenLineSegmentAndAABB)
    {
        Vec2 lineStart(0.0f, 0.0f), lineEnd(1.0f, 0.0f);
        AABB2 aabb(Vec2(2.0f, -1.0f), Vec2(3.0f, 1.0f));
        float dist = GeometryUtils::GetShortestDistanceBetweenLineSegmentAndAABB(lineStart, lineEnd, aabb);
        EXPECT_NEAR(dist, 1.0f, 1e-6f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetShortestDistanceSquaredBetweenLineSegmentAndAABB
    //
    TEST(GeometryUtils, GetShortestDistanceSquaredBetweenLineSegmentAndAABB)
    {
        Vec2 lineStart(0.0f, 0.0f), lineEnd(1.0f, 0.0f);
        AABB2 aabb(Vec2(2.0f, -1.0f), Vec2(3.0f, 1.0f));
        float distSq = GeometryUtils::GetShortestDistanceSquaredBetweenLineSegmentAndAABB(lineStart, lineEnd, aabb);
        EXPECT_NEAR(distSq, 1.0f, 1e-6f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // SweepDiscVsPoint
    //
    TEST(GeometryUtils, SweepDiscVsPoint)
    {
        Vec2 discStart(0.0f, 0.0f);
        Vec2 discEnd(5.0f, 0.0f);
        float discRadius = 1.0f;
        Vec2 point(3.0f, 0.0f);
        float t = -1.0f;
        EXPECT_TRUE(GeometryUtils::SweepDiscVsPoint(discStart, discEnd, discRadius, point, t));
        EXPECT_GE(t, 0.0f);
        EXPECT_LE(t, 1.0f);

        Vec2 point2(10.0f, 0.0f);
        float t2 = -1.0f;
        EXPECT_FALSE(GeometryUtils::SweepDiscVsPoint(discStart, discEnd, discRadius, point2, t2));
    }

}
