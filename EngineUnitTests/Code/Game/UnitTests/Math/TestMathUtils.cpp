// Bradley Christensen - 2022
#include <corecrt_math_defines.h>

#include "gtest/gtest.h"
#include "Game/UnitTests/TestUtils.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
TEST(MathUtils, Trig)
{
    constexpr float ALLOWABLE_ERROR = 0.00001f;
    EXPECT_NEAR(SinDegrees(0.f), 0.f, ALLOWABLE_ERROR);
    EXPECT_NEAR(CosDegrees(180.f), -1.f, ALLOWABLE_ERROR);
    
    EXPECT_NEAR(SinRadians(static_cast<float>(M_PI)), 0.f, ALLOWABLE_ERROR);
    EXPECT_NEAR(CosRadians(static_cast<float>(M_PI)), -1.f, ALLOWABLE_ERROR);
    
    EXPECT_NEAR(DegreesToRadians(180.f), static_cast<float>(M_PI), ALLOWABLE_ERROR);
    EXPECT_NEAR(RadiansToDegrees(static_cast<float>(M_PI) * 2.f), 360.f, ALLOWABLE_ERROR);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(MathUtils, Clamp)
{
    float value = 4.f;
    value = ClampF(value, 1.f, 10.f);
    EXPECT_FLOAT_EQ(value, 4.f);
    value = ClampF(value, 5.f, 10.f);
    EXPECT_FLOAT_EQ(value, 5.f);
    value = ClampF(value, 2.f, 3.f);
    EXPECT_FLOAT_EQ(value, 3.f);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(MathUtils, Lerp)
{
    EXPECT_FLOAT_EQ(Interpolate(2.f, 10.f, -1.f), -6.f);
    EXPECT_FLOAT_EQ(Interpolate(2.f, 10.f, 0.f), 2.f);
    EXPECT_FLOAT_EQ(Interpolate(2.f, 10.f, 0.5f), 6.f);
    EXPECT_FLOAT_EQ(Interpolate(2.f, 10.f, 1.f), 10.f);
    EXPECT_FLOAT_EQ(Interpolate(2.f, 10.f, 11.f), 90.f);
    
    EXPECT_FLOAT_EQ(InterpolateClamped(2.f, 10.f, -1.f), 2.f);
    EXPECT_FLOAT_EQ(InterpolateClamped(2.f, 10.f, 0.f), 2.f);
    EXPECT_FLOAT_EQ(InterpolateClamped(2.f, 10.f, 0.5f), 6.f);
    EXPECT_FLOAT_EQ(InterpolateClamped(2.f, 10.f, 1.f), 10.f);
    EXPECT_FLOAT_EQ(InterpolateClamped(2.f, 10.f, 11.f), 10.f);
    
    EXPECT_EQ(InterpolateInt(2, 10, -1.f), -6);
    EXPECT_EQ(InterpolateInt(2, 10, 0.f), 2);
    EXPECT_EQ(InterpolateInt(2, 10, 0.5f), 6);
    EXPECT_EQ(InterpolateInt(2, 10, 1.f), 10);
    EXPECT_EQ(InterpolateInt(2, 10, 11.f), 90);
    
    EXPECT_EQ(InterpolateIntClamped(2, 10, -1.f), 2);
    EXPECT_EQ(InterpolateIntClamped(2, 10, 0.f), 2);
    EXPECT_EQ(InterpolateIntClamped(2, 10, 0.5f), 6);
    EXPECT_EQ(InterpolateIntClamped(2, 10, 1.f), 10);
    EXPECT_EQ(InterpolateIntClamped(2, 10, 11.f), 10);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(MathUtils, GetLength2D)
{
    Vec2 vec(1.f, 0.f);
    EXPECT_FLOAT_EQ(GetLength2D(vec), 1.f);

    Vec2 vec2(0.f, -10.f);
    EXPECT_FLOAT_EQ(GetLength2D(vec2), 10.f);

    Vec2 vec3(3.f, -4.f);
    EXPECT_FLOAT_EQ(GetLength2D(vec3), 5.f);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(MathUtils, NormalizeVector2D)
{
    Vec2 vecZero(0.f, 0.f);
    NormalizeVector2D(vecZero);
    EXPECT_VEC2_EQf(vecZero, 0.f, 0.f);
    EXPECT_FLOAT_EQ(GetLength2D(vecZero), 0.f);

    Vec2 vec(1.f, 0.f);
    NormalizeVector2D(vec);
    EXPECT_VEC2_EQf(vec, 1.f, 0.f);
    EXPECT_FLOAT_EQ(GetLength2D(vec), 1.f);

    Vec2 vec2(0.f, -10.f);
    NormalizeVector2D(vec2);
    EXPECT_VEC2_EQf(vec2, 0.f, -1.f);
    EXPECT_FLOAT_EQ(GetLength2D(vec2), 1.f);

    Vec2 vec3(3.f, -4.f);
    NormalizeVector2D(vec3);
    EXPECT_VEC2_EQf(vec3, 0.6f, -0.8f);
    EXPECT_FLOAT_EQ(GetLength2D(vec3), 1.f);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(MathUtils, DotProduct2D)
{
    Vec2 forward(1.f, 0.f);
    Vec2 backward(-1.f, 0.f);
    Vec2 left(0.f, 1.f);
    Vec2 right(0.f, -1.f);

    // Same
    EXPECT_FLOAT_EQ(DotProduct2D(forward, forward), 1.f);
    EXPECT_FLOAT_EQ(DotProduct2D(left, left), 1.f);
    EXPECT_FLOAT_EQ(DotProduct2D(right, right), 1.f);
    EXPECT_FLOAT_EQ(DotProduct2D(backward, backward), 1.f);

    // Opposite
    EXPECT_FLOAT_EQ(DotProduct2D(forward, backward), -1.f);
    EXPECT_FLOAT_EQ(DotProduct2D(backward, forward), -1.f);
    EXPECT_FLOAT_EQ(DotProduct2D(left, right), -1.f);
    EXPECT_FLOAT_EQ(DotProduct2D(right, left), -1.f);

    // Adjacent
    EXPECT_FLOAT_EQ(DotProduct2D(forward, left), 0.f);
    EXPECT_FLOAT_EQ(DotProduct2D(forward, right), 0.f);
    EXPECT_FLOAT_EQ(DotProduct2D(left, forward), 0.f);
    EXPECT_FLOAT_EQ(DotProduct2D(right, forward), 0.f);
    EXPECT_FLOAT_EQ(DotProduct2D(backward, left), 0.f);
    EXPECT_FLOAT_EQ(DotProduct2D(backward, right), 0.f);
    EXPECT_FLOAT_EQ(DotProduct2D(left, backward), 0.f);
    EXPECT_FLOAT_EQ(DotProduct2D(right, backward), 0.f);

    // Random
    EXPECT_FLOAT_EQ(DotProduct2D(Vec2(), Vec2::ZeroVector), 0.f);
    EXPECT_FLOAT_EQ(DotProduct2D(Vec2(1000.f, 100.f).GetNormalized(), Vec2(1000.f, 100.f).GetNormalized()), 1.f);
    EXPECT_FLOAT_EQ(DotProduct2D(Vec2(100.f, 1000.f).GetNormalized(), -Vec2(100.f, 1000.f).GetNormalized()), -1.f);
    EXPECT_FLOAT_EQ(DotProduct2D(Vec2(0.543f, -0.123f), Vec2(-0.954f, -0.543f)), -0.451233f);
}
