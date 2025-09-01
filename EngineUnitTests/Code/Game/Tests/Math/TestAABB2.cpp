// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
// AABB2 Unit Tests
//
namespace TestAABB2
{

    //----------------------------------------------------------------------------------------------------------------------
    // Default Constructor and ZeroToOne
    //
    TEST(AABB2, DefaultConstructorAndZeroToOne)
    {
        AABB2 a;
        EXPECT_EQ(a.mins, Vec2(0.f, 0.f));
        EXPECT_EQ(a.maxs, Vec2(1.f, 1.f));

        EXPECT_EQ(AABB2::ZeroToOne.mins, Vec2(0.f, 0.f));
        EXPECT_EQ(AABB2::ZeroToOne.maxs, Vec2(1.f, 1.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Constructor Vec2, Vec2
    //
    TEST(AABB2, ConstructorVec2Vec2)
    {
        Vec2 min(1.f, 2.f);
        Vec2 max(3.f, 4.f);
        AABB2 a(min, max);
        EXPECT_EQ(a.mins, min);
        EXPECT_EQ(a.maxs, max);

        // Swapped order
        AABB2 b(max, min);
        EXPECT_EQ(b.mins, min);
        EXPECT_EQ(b.maxs, max);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Constructor float, float, float, float
    //
    TEST(AABB2, ConstructorFloats)
    {
        AABB2 a(1.f, 2.f, 3.f, 4.f);
        EXPECT_EQ(a.mins, Vec2(1.f, 2.f));
        EXPECT_EQ(a.maxs, Vec2(3.f, 4.f));

        // Swapped order
        AABB2 b(3.f, 4.f, 1.f, 2.f);
        EXPECT_EQ(b.mins, Vec2(1.f, 2.f));
        EXPECT_EQ(b.maxs, Vec2(3.f, 4.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Constructor center, halfWidth, halfHeight
    //
    TEST(AABB2, ConstructorCenterHalfDims)
    {
        Vec2 center(5.f, 5.f);
        float hw = 2.f, hh = 3.f;
        AABB2 a(center, hw, hh);
        EXPECT_EQ(a.mins, Vec2(3.f, 2.f));
        EXPECT_EQ(a.maxs, Vec2(7.f, 8.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetCenter
    //
    TEST(AABB2, GetCenter)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        EXPECT_EQ(a.GetCenter(), Vec2(3.f, 4.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetDimensions
    //
    TEST(AABB2, GetDimensions)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        EXPECT_EQ(a.GetDimensions(), Vec2(4.f, 4.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetHalfDimensions
    //
    TEST(AABB2, GetHalfDimensions)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        EXPECT_EQ(a.GetHalfDimensions(), Vec2(2.f, 2.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetWidth
    //
    TEST(AABB2, GetWidth)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        EXPECT_FLOAT_EQ(a.GetWidth(), 4.f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetHeight
    //
    TEST(AABB2, GetHeight)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        EXPECT_FLOAT_EQ(a.GetHeight(), 4.f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetHalfHeight
    //
    TEST(AABB2, GetHalfHeight)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        EXPECT_FLOAT_EQ(a.GetHalfHeight(), 2.f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetAspect
    //
    TEST(AABB2, GetAspect)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        EXPECT_FLOAT_EQ(a.GetAspect(), 1.f);

        AABB2 b(Vec2(0.f, 0.f), Vec2(8.f, 2.f));
        EXPECT_FLOAT_EQ(b.GetAspect(), 4.f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetTopLeft
    //
    TEST(AABB2, GetTopLeft)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        EXPECT_EQ(a.GetTopLeft(), Vec2(1.f, 6.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetBottomRight
    //
    TEST(AABB2, GetBottomRight)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        EXPECT_EQ(a.GetBottomRight(), Vec2(5.f, 2.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetCenterLeft
    //
    TEST(AABB2, GetCenterLeft)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        EXPECT_EQ(a.GetCenterLeft(), Vec2(1.f, 4.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetCenterRight
    //
    TEST(AABB2, GetCenterRight)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        EXPECT_EQ(a.GetCenterRight(), Vec2(5.f, 4.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetExpandedBy
    //
    TEST(AABB2, GetExpandedBy)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        AABB2 b = a.GetExpandedBy(2.f);
        EXPECT_EQ(b.mins, Vec2(-1.f, 0.f));
        EXPECT_EQ(b.maxs, Vec2(7.f, 8.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetNearestPoint
    //
    TEST(AABB2, GetNearestPoint)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        // Inside
        EXPECT_EQ(a.GetNearestPoint(Vec2(3.f, 4.f)), Vec2(3.f, 4.f));
        // Outside left
        EXPECT_EQ(a.GetNearestPoint(Vec2(-1.f, 4.f)), Vec2(1.f, 4.f));
        // Outside right
        EXPECT_EQ(a.GetNearestPoint(Vec2(10.f, 4.f)), Vec2(5.f, 4.f));
        // Outside bottom
        EXPECT_EQ(a.GetNearestPoint(Vec2(3.f, 0.f)), Vec2(3.f, 2.f));
        // Outside top
        EXPECT_EQ(a.GetNearestPoint(Vec2(3.f, 10.f)), Vec2(3.f, 6.f));
        // Outside corner
        EXPECT_EQ(a.GetNearestPoint(Vec2(-1.f, 10.f)), Vec2(1.f, 6.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // IsPointInside
    //
    TEST(AABB2, IsPointInside)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        EXPECT_TRUE(a.IsPointInside(Vec2(2.f, 3.f)));
        EXPECT_FALSE(a.IsPointInside(Vec2(1.f, 3.f))); // on left edge
        EXPECT_FALSE(a.IsPointInside(Vec2(5.f, 3.f))); // on right edge
        EXPECT_FALSE(a.IsPointInside(Vec2(3.f, 2.f))); // on bottom edge
        EXPECT_FALSE(a.IsPointInside(Vec2(3.f, 6.f))); // on top edge
        EXPECT_FALSE(a.IsPointInside(Vec2(0.f, 0.f)));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // IsOverlapping
    //
    TEST(AABB2, IsOverlapping)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        AABB2 b(Vec2(4.f, 5.f), Vec2(8.f, 9.f));
        EXPECT_TRUE(a.IsOverlapping(b));
        EXPECT_TRUE(b.IsOverlapping(a));

        AABB2 c(Vec2(6.f, 7.f), Vec2(8.f, 9.f));
        EXPECT_FALSE(a.IsOverlapping(c));
        EXPECT_FALSE(c.IsOverlapping(a));

        // Touching at edge
        AABB2 d(Vec2(5.f, 2.f), Vec2(7.f, 6.f));
        EXPECT_TRUE(a.IsOverlapping(d));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Translate
    //
    TEST(AABB2, Translate)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        a.Translate(Vec2(2.f, 3.f));
        EXPECT_EQ(a.mins, Vec2(3.f, 5.f));
        EXPECT_EQ(a.maxs, Vec2(7.f, 9.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // SetCenter
    //
    TEST(AABB2, SetCenter)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        a.SetCenter(Vec2(10.f, 20.f));
        EXPECT_EQ(a.GetCenter(), Vec2(10.f, 20.f));
        EXPECT_EQ(a.GetDimensions(), Vec2(4.f, 4.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // SetDimsAboutCenter
    //
    TEST(AABB2, SetDimsAboutCenter)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        a.SetDimsAboutCenter(Vec2(10.f, 8.f));
        EXPECT_EQ(a.GetDimensions(), Vec2(10.f, 8.f));
        EXPECT_EQ(a.GetCenter(), Vec2(3.f, 4.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // SetHalfDimsAboutCenter
    //
    TEST(AABB2, SetHalfDimsAboutCenter)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        a.SetHalfDimsAboutCenter(Vec2(2.f, 3.f));
        EXPECT_EQ(a.GetDimensions(), Vec2(4.f, 6.f));
        EXPECT_EQ(a.GetCenter(), Vec2(3.f, 4.f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Squeeze
    //
    TEST(AABB2, Squeeze)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        a.Squeeze(1.f);
        EXPECT_EQ(a.mins, Vec2(2.f, 3.f));
        EXPECT_EQ(a.maxs, Vec2(4.f, 5.f));

        // Squeeze to a point
        AABB2 b(Vec2(1.f, 2.f), Vec2(2.f, 3.f));
        b.Squeeze(2.f);
        EXPECT_EQ(b.mins, b.maxs);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // ExpandBy
    //
    TEST(AABB2, ExpandBy)
    {
        AABB2 a(Vec2(1.f, 2.f), Vec2(5.f, 6.f));
        a.ExpandBy(2.f);
        EXPECT_EQ(a.mins, Vec2(-1.f, 0.f));
        EXPECT_EQ(a.maxs, Vec2(7.f, 8.f));
    }

}
