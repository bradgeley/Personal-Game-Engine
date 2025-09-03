// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Math/MathUtils.h"
#include "gtest/gtest.h"



//----------------------------------------------------------------------------------------------------------------------
// MathUtils Unit Tests
//
namespace TestMathUtils
{
    using namespace MathUtils;

    //----------------------------------------------------------------------------------------------------------------------
    // Clamp
    //
    TEST(MathUtils, Clamp)
    {
        EXPECT_EQ(Clamp(5.0, 1.0, 10.0), 5.0);
        EXPECT_EQ(Clamp(-1, 0, 10), 0);
        EXPECT_EQ(Clamp(15, 0, 10), 10);
        EXPECT_FLOAT_EQ(Clamp(2.5f, 1.0f, 3.0f), 2.5f);
        EXPECT_FLOAT_EQ(Clamp(-2.0f, -1.0f, 1.0f), -1.0f);
        EXPECT_FLOAT_EQ(Clamp(2.0f, -1.0f, 1.0f), 1.0f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Clamp01
    //
    TEST(MathUtils, Clamp01)
    {
        EXPECT_FLOAT_EQ(Clamp01F(0.5f), 0.5f);
        EXPECT_FLOAT_EQ(Clamp01F(-0.1f), 0.0f);
        EXPECT_FLOAT_EQ(Clamp01F(1.1f), 1.0f);
        EXPECT_FLOAT_EQ(Clamp01F(0.0f), 0.0f);
        EXPECT_FLOAT_EQ(Clamp01F(1.0f), 1.0f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Lerp
    //
    TEST(MathUtils, Lerp)
    {
        EXPECT_FLOAT_EQ(Interpolate(0.0f, 10.0f, 0.0f), 0.0f);
        EXPECT_FLOAT_EQ(Interpolate(0.0f, 10.0f, 1.0f), 10.0f);
        EXPECT_FLOAT_EQ(Interpolate(0.0f, 10.0f, 0.5f), 5.0f);
        EXPECT_FLOAT_EQ(Interpolate(-5.0f, 5.0f, 0.5f), 0.0f);
        EXPECT_FLOAT_EQ(Interpolate(10.0f, 20.0f, 0.25f), 12.5f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // LerpClamped
    //
    TEST(MathUtils, LerpClamped)
    {
        EXPECT_FLOAT_EQ(InterpolateClamped(0.0f, 10.0f, -1.0f), 0.0f);
        EXPECT_FLOAT_EQ(InterpolateClamped(0.0f, 10.0f, 2.0f), 10.0f);
        EXPECT_FLOAT_EQ(InterpolateClamped(0.0f, 10.0f, 0.5f), 5.0f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // RangeMap
    //
    TEST(MathUtils, RangeMap)
    {
        EXPECT_FLOAT_EQ(RangeMap(5.0f, 0.0f, 10.0f, 0.0f, 100.0f), 50.0f);
        EXPECT_FLOAT_EQ(RangeMap(0.0f, 0.0f, 10.0f, 0.0f, 100.0f), 0.0f);
        EXPECT_FLOAT_EQ(RangeMap(10.0f, 0.0f, 10.0f, 0.0f, 100.0f), 100.0f);
        EXPECT_FLOAT_EQ(RangeMap(-5.0f, -10.0f, 0.0f, 0.0f, 1.0f), 0.5f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // RangeMapClamped
    //
    TEST(MathUtils, RangeMapClamped)
    {
        EXPECT_FLOAT_EQ(RangeMapClamped(5.0f, 0.0f, 10.0f, 0.0f, 100.0f), 50.0f);
        EXPECT_FLOAT_EQ(RangeMapClamped(-5.0f, 0.0f, 10.0f, 0.0f, 100.0f), 0.0f);
        EXPECT_FLOAT_EQ(RangeMapClamped(15.0f, 0.0f, 10.0f, 0.0f, 100.0f), 100.0f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Interpolate
    //
    TEST(MathUtils, Interpolate)
    {
        EXPECT_FLOAT_EQ(Interpolate(0.0f, 10.0f, 0.0f), 0.0f);
        EXPECT_FLOAT_EQ(Interpolate(0.0f, 10.0f, 1.0f), 10.0f);
        EXPECT_FLOAT_EQ(Interpolate(0.0f, 10.0f, 0.5f), 5.0f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // InterpolateClamped
    //
    TEST(MathUtils, InterpolateClamped)
    {
        EXPECT_FLOAT_EQ(InterpolateClamped(0.0f, 10.0f, -1.0f), 0.0f);
        EXPECT_FLOAT_EQ(InterpolateClamped(0.0f, 10.0f, 2.0f), 10.0f);
        EXPECT_FLOAT_EQ(InterpolateClamped(0.0f, 10.0f, 0.5f), 5.0f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // RoundDowFloornToInt
    //
    TEST(MathUtils, Floor)
    {
        EXPECT_EQ(FloorF(3.7f), 3);
        EXPECT_EQ(FloorF(-1.2f), -2);
        EXPECT_EQ(FloorF(0.0f), 0);
        EXPECT_EQ(FloorF(2.0f), 2);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // RoundUpToInt
    //
    TEST(MathUtils, Ceiling)
    {
        EXPECT_EQ(CeilingF(3.2f), 4);
        EXPECT_EQ(CeilingF(-1.8f), -1);
        EXPECT_EQ(CeilingF(0.0f), 0);
        EXPECT_EQ(CeilingF(2.0f), 2);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // RoundToNearestInt
    //
    TEST(MathUtils, Round)
    {
        EXPECT_EQ(RoundF(3.2f), 3);
        EXPECT_EQ(RoundF(3.7f), 4);
        EXPECT_EQ(RoundF(-1.2f), -1);
        EXPECT_EQ(RoundF(-1.8f), -2);
        EXPECT_EQ(RoundF(0.0f), 0);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // IsNearlyEqual
    //
    TEST(MathUtils, IsNearlyEqual)
    {
        EXPECT_TRUE(IsNearlyEqual(1.0f, 1.0f));
        EXPECT_TRUE(IsNearlyEqual(1.0f, 1.000001f, 1e-5f));
        EXPECT_FALSE(IsNearlyEqual(1.0f, 1.1f, 1e-3f));
        EXPECT_TRUE(IsNearlyEqual(-1.0f, -1.0f));
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Sqrt
    //
    TEST(MathUtils, Sqrt)
    {
        EXPECT_FLOAT_EQ(SqrtF(4), 2);
        EXPECT_FLOAT_EQ(SqrtF(9), 3);
        EXPECT_FLOAT_EQ(SqrtF(2.25f), 1.5f);
        EXPECT_FLOAT_EQ(SqrtF(0), 0);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Sign
    //
    TEST(MathUtils, Sign)
    {
        EXPECT_EQ(Sign(5), 1);
        EXPECT_EQ(Sign(-5), -1);
        EXPECT_EQ(Sign(0), 0);
        EXPECT_EQ(Sign(3.14f), 1);
        EXPECT_EQ(Sign(-2.71f), -1);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Abs
    //
    TEST(MathUtils, Abs)
    {
        EXPECT_EQ(Abs(5), 5);
        EXPECT_EQ(Abs(-5), 5);
        EXPECT_FLOAT_EQ(Abs(3.14f), 3.14f);
        EXPECT_FLOAT_EQ(Abs(-2.71f), 2.71f);
        EXPECT_EQ(Abs(0), 0);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Min
    //
    TEST(MathUtils, Min)
    {
        EXPECT_EQ(Min(5, 10), 5);
        EXPECT_EQ(Min(-5, 0), -5);
        EXPECT_FLOAT_EQ(Min(3.14f, 2.71f), 2.71f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Max
    //
    TEST(MathUtils, Max)
    {
        EXPECT_EQ(Max(5, 10), 10);
        EXPECT_EQ(Max(-5, 0), 0);
        EXPECT_FLOAT_EQ(Max(3.14f, 2.71f), 3.14f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // DegreesToRadians
    //
    TEST(MathUtils, DegreesToRadians)
    {
        EXPECT_FLOAT_EQ(DegreesToRadians(0.0f), 0.0f);
        EXPECT_FLOAT_EQ(DegreesToRadians(180.0f), 3.1415927f);
        EXPECT_FLOAT_EQ(DegreesToRadians(360.0f), 6.2831855f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // RadiansToDegrees
    //
    TEST(MathUtils, RadiansToDegrees)
    {
        EXPECT_FLOAT_EQ(RadiansToDegrees(0.0f), 0.0f);
        EXPECT_FLOAT_EQ(RadiansToDegrees(3.1415927f), 180.0f);
        EXPECT_FLOAT_EQ(RadiansToDegrees(6.2831855f), 360.0f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // CosDegrees
    //
    TEST(MathUtils, CosDegrees)
    {
        EXPECT_NEAR(CosDegrees(0.0f), 1.0f, 1e-6f);
        EXPECT_NEAR(CosDegrees(90.0f), 0.0f, 1e-6f);
        EXPECT_NEAR(CosDegrees(180.0f), -1.0f, 1e-6f);
        EXPECT_NEAR(CosDegrees(360.0f), 1.0f, 1e-6f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // SinDegrees
    //
    TEST(MathUtils, SinDegrees)
    {
        EXPECT_NEAR(SinDegrees(0.0f), 0.0f, 1e-6f);
        EXPECT_NEAR(SinDegrees(90.0f), 1.0f, 1e-6f);
        EXPECT_NEAR(SinDegrees(180.0f), 0.0f, 1e-6f);
        EXPECT_NEAR(SinDegrees(270.0f), -1.0f, 1e-6f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // WrapInteger
    //
    TEST(MathUtils, WrapInteger)
    {
        EXPECT_EQ(WrapInteger(5, 0, 10), 5);
        EXPECT_EQ(WrapInteger(3, 10, 0), 3);
        EXPECT_EQ(WrapInteger(10, 0, 10), 0);
        EXPECT_EQ(WrapInteger(11, 0, 10), 1);
        EXPECT_EQ(WrapInteger(-1, 0, 10), 9);
        EXPECT_EQ(WrapInteger(-11, 0, 10), 9);
        EXPECT_EQ(WrapInteger(0, 0, 10), 0);
        EXPECT_EQ(WrapInteger(20, 0, 10), 0);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // GetFractionWithin
    //
    TEST(MathUtils, GetFractionWithin)
    {
        EXPECT_FLOAT_EQ(GetFractionWithinF(5.0f, 0.0f, 10.0f), 0.5f);
        EXPECT_FLOAT_EQ(GetFractionWithinF(0.0f, 0.0f, 10.0f), 0.0f);
        EXPECT_FLOAT_EQ(GetFractionWithinF(10.0f, 0.0f, 10.0f), 1.0f);
        EXPECT_FLOAT_EQ(GetFractionWithinF(-5.0f, -10.0f, 0.0f), 0.5f);
    }

}
