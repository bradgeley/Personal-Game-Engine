// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Math/Vec3.h"



//----------------------------------------------------------------------------------------------------------------------
// Vec3 Unit Tests
//
namespace TestVec3
{

    //----------------------------------------------------------------------------------------------------------------------
    // Addition
    //
    TEST(Vec3, Addition)
    {
        Vec3 a(1.0f, 2.0f, 3.0f);
        Vec3 b(4.0f, 5.0f, 6.0f);
        Vec3 c = a + b;
        EXPECT_EQ(c, Vec3(5.0f, 7.0f, 9.0f));

        Vec3 d(-1.0f, -2.0f, -3.0f);
        Vec3 e(1.0f, 2.0f, 3.0f);
        EXPECT_EQ(d + e, Vec3(0.0f, 0.0f, 0.0f));

        Vec3 f(1e6f, -1e6f, 1e-6f);
        Vec3 g(-1e6f, 1e6f, -1e-6f);
        EXPECT_EQ(f + g, Vec3(0.0f, 0.0f, 0.0f));
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Subtraction
    //
    TEST(Vec3, Subtraction)
    {
        Vec3 a(5.0f, 7.0f, 9.0f);
        Vec3 b(1.0f, 2.0f, 3.0f);
        Vec3 c = a - b;
        EXPECT_EQ(c, Vec3(4.0f, 5.0f, 6.0f));

        Vec3 d(-1.0f, -2.0f, -3.0f);
        Vec3 e(1.0f, 2.0f, 3.0f);
        EXPECT_EQ(d - e, Vec3(-2.0f, -4.0f, -6.0f));

        Vec3 f(1e6f, 1e-6f, -1e6f);
        Vec3 g(-1e6f, -1e-6f, 1e6f);
        EXPECT_EQ(f - g, Vec3(2e6f, 2e-6f, -2e6f));
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Scalar Multiplication
    //
    TEST(Vec3, ScalarMultiplication)
    {
        Vec3 a(1.0f, 2.0f, 3.0f);
        Vec3 b = a * 2.0f;
        EXPECT_EQ(b, Vec3(2.0f, 4.0f, 6.0f));

        Vec3 c(-1.0f, -2.0f, -3.0f);
        EXPECT_EQ(c * -2.0f, Vec3(2.0f, 4.0f, 6.0f));

        Vec3 d(1e-3f, 1e3f, -1e3f);
        EXPECT_EQ(d * 1e3f, Vec3(1.0f, 1e6f, -1e6f));

        // Test float * Vec3
        Vec3 e = a * 3.0f;
        EXPECT_EQ(e, Vec3(3.0f, 6.0f, 9.0f));
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Scalar Division
    //
    TEST(Vec3, ScalarDivision)
    {
        Vec3 a(2.0f, 4.0f, 6.0f);
        Vec3 b = a / 2.0f;
        EXPECT_EQ(b, Vec3(1.0f, 2.0f, 3.0f));

        Vec3 c(-2.0f, -4.0f, -6.0f);
        EXPECT_EQ(c / -2.0f, Vec3(1.0f, 2.0f, 3.0f));

        Vec3 d(1e6f, 1e-6f, -1e6f);
        EXPECT_EQ(d / 1e2f, Vec3(1e4f, 1e-8f, -1e4f));
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Dot Product
    //
    TEST(Vec3, DotProduct)
    {
        Vec3 a(1.0f, 2.0f, 3.0f);
        Vec3 b(4.0f, 5.0f, 6.0f);
        float dot = a.Dot(b);
        EXPECT_EQ(dot, 32.0f);

        Vec3 c(-1.0f, -2.0f, -3.0f);
        Vec3 d(1.0f, 2.0f, 3.0f);
        EXPECT_EQ(c.Dot(d), -14.0f);

        Vec3 e(1e3f, 1e-3f, 2.0f);
        Vec3 f(1e-3f, 1e3f, 2.0f);
        EXPECT_FLOAT_EQ(e.Dot(f), 1e3f * 1e-3f + 1e-3f * 1e3f + 2.0f * 2.0f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Cross Product
    //
    TEST(Vec3, CrossProduct)
    {
        Vec3 a(1.0f, 0.0f, 0.0f);
        Vec3 b(0.0f, 1.0f, 0.0f);
        EXPECT_EQ(a.Cross(b), Vec3(0.0f, 0.0f, 1.0f));

        Vec3 c(2.0f, 3.0f, 4.0f);
        Vec3 d(5.0f, 6.0f, 7.0f);
        EXPECT_EQ(c.Cross(d), Vec3(-3.0f, 6.0f, -3.0f));

        Vec3 e(1e3f, 0.0f, 0.0f);
        Vec3 f(0.0f, 1e3f, 0.0f);
        EXPECT_EQ(e.Cross(f), Vec3(0.0f, 0.0f, 1e6f));
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Length
    //
    TEST(Vec3, GetLength)
    {
        Vec3 a(1.0f, 2.0f, 2.0f);
        EXPECT_FLOAT_EQ(a.GetLength(), 3.0f);

        Vec3 b(-2.0f, -3.0f, -6.0f);
        EXPECT_FLOAT_EQ(b.GetLength(), 7.0f);

        Vec3 c(1e-3f, 0.0f, 0.0f);
        EXPECT_FLOAT_EQ(c.GetLength(), 1e-3f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // LengthSquared
    //
    TEST(Vec3, GetLengthSquared)
    {
        Vec3 a(1.0f, 2.0f, 2.0f);
        EXPECT_FLOAT_EQ(a.GetLengthSquared(), 9.0f);

        Vec3 b(-2.0f, -3.0f, -6.0f);
        EXPECT_FLOAT_EQ(b.GetLengthSquared(), 49.0f);

        Vec3 c(1e-3f, 0.0f, 0.0f);
        EXPECT_FLOAT_EQ(c.GetLengthSquared(), 1e-6f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // Normalize
    //
    TEST(Vec3, Normalize)
    {
        Vec3 a(0.0f, 3.0f, 4.0f);
        a.Normalize();
        EXPECT_NEAR(a.x, 0.0f, 1e-6f);
        EXPECT_NEAR(a.y, 0.6f, 1e-6f);
        EXPECT_NEAR(a.z, 0.8f, 1e-6f);

        Vec3 b(-5.0f, 0.0f, 0.0f);
        b.Normalize();
        EXPECT_NEAR(b.x, -1.0f, 1e-6f);
        EXPECT_NEAR(b.y, 0.0f, 1e-6f);
        EXPECT_NEAR(b.z, 0.0f, 1e-6f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // GetNormalized
    //
    TEST(Vec3, GetNormalized)
    {
        Vec3 a(0.0f, 3.0f, 4.0f);
        Vec3 g = a.GetNormalized();
        EXPECT_NEAR(g.x, 0.0f, 1e-6f);
        EXPECT_NEAR(g.y, 0.6f, 1e-6f);
        EXPECT_NEAR(g.z, 0.8f, 1e-6f);

        Vec3 b(0.0f, -2.0f, 0.0f);
        Vec3 h = b.GetNormalized();
        EXPECT_NEAR(h.x, 0.0f, 1e-6f);
        EXPECT_NEAR(h.y, -1.0f, 1e-6f);
        EXPECT_NEAR(h.z, 0.0f, 1e-6f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // DistanceTo
    //
    TEST(Vec3, GetDistanceTo)
    {
        Vec3 a(0.0f, 0.0f, 0.0f);
        Vec3 b(1.0f, 2.0f, 2.0f);
        EXPECT_FLOAT_EQ(a.GetDistanceTo(b), 3.0f);

        Vec3 c(-1.0f, -1.0f, -1.0f);
        Vec3 d(2.0f, 2.0f, 2.0f);
        EXPECT_FLOAT_EQ(c.GetDistanceTo(d), sqrtf(27.0f));
    }


    //----------------------------------------------------------------------------------------------------------------------
    // DistanceSquaredTo
    //
    TEST(Vec3, GetDistanceSquaredTo)
    {
        Vec3 a(0.0f, 0.0f, 0.0f);
        Vec3 b(1.0f, 2.0f, 2.0f);
        EXPECT_FLOAT_EQ(a.GetDistanceSquaredTo(b), 9.0f);

        Vec3 c(-1.0f, -1.0f, -1.0f);
        Vec3 d(2.0f, 2.0f, 2.0f);
        EXPECT_FLOAT_EQ(c.GetDistanceSquaredTo(d), 27.0f);
    }


    //----------------------------------------------------------------------------------------------------------------------
    // IsZero
    //
    TEST(Vec3, IsZero)
    {
        Vec3 a(0.0f, 0.0f, 0.0f);
        EXPECT_TRUE(a.IsZero());

        Vec3 b(1e-6f, 0.0f, 0.0f);
        EXPECT_FALSE(b.IsZero());
    }


    //----------------------------------------------------------------------------------------------------------------------
    // IsNearlyZero
    //
    TEST(Vec3, IsNearlyZero)
    {
        Vec3 a(1e-7f, -1e-7f, 1e-7f);
        EXPECT_TRUE(a.IsNearlyZero(1e-6f));

        Vec3 b(1e-3f, 0.0f, 0.0f);
        EXPECT_FALSE(b.IsNearlyZero(1e-6f));
    }


    //----------------------------------------------------------------------------------------------------------------------
    // NegationOperator
    //
    TEST(Vec3, NegationOperator)
    {
        Vec3 a(3.0f, -4.0f, 5.0f);
        Vec3 b = -a;
        EXPECT_EQ(b, Vec3(-3.0f, 4.0f, -5.0f));
    }


    //----------------------------------------------------------------------------------------------------------------------
    // ComponentWiseMultiplication
    //
    TEST(Vec3, ComponentWiseMultiplication)
    {
        Vec3 a(2.0f, 3.0f, 4.0f);
        Vec3 b(4.0f, -1.0f, 0.5f);
        EXPECT_EQ(a * b, Vec3(8.0f, -3.0f, 2.0f));
    }


    //----------------------------------------------------------------------------------------------------------------------
    // ComponentWiseDivision
    //
    TEST(Vec3, ComponentWiseDivision)
    {
        Vec3 a(8.0f, -3.0f, 2.0f);
        Vec3 b(2.0f, 3.0f, 0.5f);
        EXPECT_EQ(a / b, Vec3(4.0f, -1.0f, 4.0f));
    }


    //----------------------------------------------------------------------------------------------------------------------
    // CompoundAssignmentOperators
    //
    TEST(Vec3, CompoundAssignmentOperators)
    {
        Vec3 a(1.0f, 2.0f, 3.0f);
        Vec3 b(3.0f, 4.0f, 5.0f);
        a += b;
        EXPECT_EQ(a, Vec3(4.0f, 6.0f, 8.0f));

        a -= b;
        EXPECT_EQ(a, Vec3(1.0f, 2.0f, 3.0f));

        a *= b;
        EXPECT_EQ(a, Vec3(3.0f, 8.0f, 15.0f));

        a /= b;
        EXPECT_EQ(a, Vec3(1.0f, 2.0f, 3.0f));

        a *= 2.0f;
        EXPECT_EQ(a, Vec3(2.0f, 4.0f, 6.0f));

        a /= 2.0f;
        EXPECT_EQ(a, Vec3(1.0f, 2.0f, 3.0f));
    }


    //----------------------------------------------------------------------------------------------------------------------
    // EqualityOperators
    //
    TEST(Vec3, EqualityOperators)
    {
        Vec3 a(1.0f, 2.0f, 3.0f);
        Vec3 b(1.0f, 2.0f, 3.0f);
        Vec3 c(2.0f, 1.0f, 3.0f);
        EXPECT_TRUE(a == b);
        EXPECT_FALSE(a != b);
        EXPECT_FALSE(a == c);
        EXPECT_TRUE(a != c);
    }

}