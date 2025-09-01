// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Math/Plane2.h"
#include "Engine/Math/Vec2.h"
#include "gtest/gtest.h"



//----------------------------------------------------------------------------------------------------------------------
// Plane2 Unit Tests
//
namespace TestPlane2
{

    //----------------------------------------------------------------------------------------------------------------------
    // Constructor
    //
    TEST(Plane2, Constructor)
    {
        Vec2 n(1.0f, 0.0f);
        Plane2 p2(n, 5.0f);
        EXPECT_EQ(p2.m_normal, n.GetNormalized());
        EXPECT_FLOAT_EQ(p2.m_distance, 5.0f);

        Plane2 p3(Vec2(0.0f, 1.0f), -3.0f);
        EXPECT_EQ(p3.m_normal, Vec2(0.0f, 1.0f));
        EXPECT_FLOAT_EQ(p3.m_distance, -3.0f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetAltitude
    //
    TEST(Plane2, GetAltitude)
    {
        Plane2 p(Vec2(0.0f, 1.0f), 2.0f);
        Vec2 pt1(0.0f, 5.0f);
        Vec2 pt2(0.0f, 2.0f);
        Vec2 pt3(0.0f, -1.0f);
        EXPECT_FLOAT_EQ(p.GetAltitude(pt1), 3.0f);
        EXPECT_FLOAT_EQ(p.GetAltitude(pt2), 0.0f);
        EXPECT_FLOAT_EQ(p.GetAltitude(pt3), -3.0f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetDistance
    //
    TEST(Plane2, GetDistance)
    {
        Plane2 p(Vec2(1.0f, 0.0f), -4.0f);
        Vec2 pt1(2.0f, 0.0f);
        Vec2 pt2(-4.0f, 0.0f);
        Vec2 pt3(-10.0f, 0.0f);
        EXPECT_FLOAT_EQ(p.GetDistance(pt1), 6.0f);
        EXPECT_FLOAT_EQ(p.GetDistance(pt2), 0.0f);
        EXPECT_FLOAT_EQ(p.GetDistance(pt3), 6.0f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Straddles
    //
    TEST(Plane2, Straddles)
    {
        Plane2 p(Vec2(0.0f, 1.0f), 0.0f);
        Vec2 a(1.0f, 1.0f);
        Vec2 b(1.0f, -1.0f);
        Vec2 c(1.0f, 2.0f);
        Vec2 d(1.0f, 3.0f);
        EXPECT_TRUE(p.Straddles(a, b));
        EXPECT_FALSE(p.Straddles(a, c));
        EXPECT_FALSE(p.Straddles(c, d));
        EXPECT_FALSE(p.Straddles(a, a));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetAltitude
    //
    TEST(Plane2, EqualityOperators)
    {
        Plane2 p1(Vec2(0.0f, 1.0f), 2.0f);
        Plane2 p2(Vec2(0.0f, 1.0f), 2.0f);
        Plane2 p3(Vec2(1.0f, 0.0f), 2.0f);
        Plane2 p4(Vec2(0.0f, 1.0f), 3.0f);

        EXPECT_TRUE(p1 == p2);
        EXPECT_FALSE(p1 != p2);
        EXPECT_FALSE(p1 == p3);
        EXPECT_TRUE(p1 != p3);
        EXPECT_FALSE(p1 == p4);
        EXPECT_TRUE(p1 != p4);
    }


}
