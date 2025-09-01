// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Math/IntVec2.h"



//----------------------------------------------------------------------------------------------------------------------
// IntVec2 Unit Tests
//
namespace TestIntVec2
{

    //----------------------------------------------------------------------------------------------------------------------
    // Constructor
    //
    TEST(IntVec2, Constructor)
    {
        IntVec2 a;
        EXPECT_EQ(a.x, 0);
        EXPECT_EQ(a.y, 0);

        IntVec2 b(3, 4);
        EXPECT_EQ(b.x, 3);
        EXPECT_EQ(b.y, 4);

        IntVec2 c(b);
        EXPECT_EQ(c.x, 3);
        EXPECT_EQ(c.y, 4);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Assignment Operator
    //
    TEST(IntVec2, AssignmentOperator)
    {
        IntVec2 a(1, 2);
        IntVec2 b;
        b = a;
        EXPECT_EQ(b.x, 1);
        EXPECT_EQ(b.y, 2);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Equality Operators
    //
    TEST(IntVec2, EqualityOperators)
    {
        IntVec2 a(1, 2);
        IntVec2 b(1, 2);
        IntVec2 c(2, 1);
        EXPECT_TRUE(a == b);
        EXPECT_FALSE(a != b);
        EXPECT_FALSE(a == c);
        EXPECT_TRUE(a != c);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Addition
    //
    TEST(IntVec2, Addition)
    {
        IntVec2 a(3, 4);
        IntVec2 b(1, 2);
        IntVec2 c = a + b;
        EXPECT_EQ(c, IntVec2(4, 6));

        IntVec2 d(-5, 10);
        IntVec2 e(2, -3);
        EXPECT_EQ(d + e, IntVec2(-3, 7));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Subtraction
    //
    TEST(IntVec2, Subtraction)
    {
        IntVec2 a(3, 4);
        IntVec2 b(1, 2);
        IntVec2 c = a - b;
        EXPECT_EQ(c, IntVec2(2, 2));

        IntVec2 d(-5, 10);
        IntVec2 e(2, -3);
        EXPECT_EQ(d - e, IntVec2(-7, 13));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Scalar Multiplication
    //
    TEST(IntVec2, ScalarMultiplication)
    {
        IntVec2 a(3, 4);
        IntVec2 b = a * 2;
        EXPECT_EQ(b, IntVec2(6, 8));

        IntVec2 c(-5, 10);
        EXPECT_EQ(c * -2, IntVec2(10, -20));

        IntVec2 d = a * 3;
        EXPECT_EQ(d, IntVec2(9, 12));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Scalar Division
    //
    TEST(IntVec2, ScalarDivision)
    {
        IntVec2 a(6, 8);
        IntVec2 b = a / 2;
        EXPECT_EQ(b, IntVec2(3, 4));

        IntVec2 c(-10, 20);
        EXPECT_EQ(c / -5, IntVec2(2, -4));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Component-wise Multiplication
    //
    TEST(IntVec2, ComponentWiseMultiplication)
    {
        IntVec2 a(2, 3);
        IntVec2 b(4, -1);
        EXPECT_EQ(a * b, IntVec2(8, -3));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Component-wise Division
    //
    TEST(IntVec2, ComponentWiseDivision)
    {
        IntVec2 a(8, -3);
        IntVec2 b(2, 3);
        EXPECT_EQ(a / b, IntVec2(4, -1));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Compound Assignment Operators
    //
    TEST(IntVec2, CompoundAssignmentOperators)
    {
        IntVec2 a(1, 2);
        IntVec2 b(3, 4);
        a += b;
        EXPECT_EQ(a, IntVec2(4, 6));

        a -= b;
        EXPECT_EQ(a, IntVec2(1, 2));

        a *= b;
        EXPECT_EQ(a, IntVec2(3, 8));

        a /= b;
        EXPECT_EQ(a, IntVec2(1, 2));

        a *= 2;
        EXPECT_EQ(a, IntVec2(2, 4));

        a /= 2;
        EXPECT_EQ(a, IntVec2(1, 2));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Negation
    //
    TEST(IntVec2, NegationOperator)
    {
        IntVec2 a(3, -4);
        IntVec2 b = -a;
        EXPECT_EQ(b, IntVec2(-3, 4));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // IsZero
    //
    TEST(IntVec2, IsZero)
    {
        IntVec2 a(0, 0);
        EXPECT_TRUE(a.IsZero());

        IntVec2 b(1, 0);
        EXPECT_FALSE(b.IsZero());
    }

}
