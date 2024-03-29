﻿// Bradley Christensen - 2022
#include "gtest/gtest.h"
#include "Game/UnitTests/TestUtils.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
TEST(Vec2, Construct)
{
    // Construction
    Vec2 vec(5.f, -5.f);
    EXPECT_VEC2_EQf(vec, 5, -5.f);

    // Copy construction
    Vec2 vec2(vec);
    EXPECT_VEC2_EQf(vec2, 5, -5.f);

    // operator=
    Vec2 vec3 = vec;
    EXPECT_VEC2_EQf(vec3, 5, -5.f);

    // negation
    Vec2 vec4 = -vec3;
    EXPECT_VEC2_EQf(vec4, -5, 5.f);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(Vec2, Equality)
{
    // Construction
    Vec2 vec(5.f, -5.f);
    Vec2 vec2(5.f, -5.f);
    EXPECT_TRUE(vec == vec2);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(Vec2, Statics)
{
    EXPECT_VEC2_EQf(Vec2::ZeroVector, 0.f, 0.f);
    EXPECT_VEC2_EQf(Vec2::ZeroToOne, 0.f, 1.f);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(Vec2, Add)
{
    // Vec + Vec 
    Vec2 vec(5.f, -5.f);
    Vec2 vec2(25.f, -7.f);
    Vec2 vec3 = vec + vec2;
    EXPECT_VEC2_EQf(vec3, 30.f, -12.f);

    // Vec += Vec
    vec3 += vec;
    EXPECT_VEC2_EQf(vec3, 35.f, -17.f);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(Vec2, Subtract)
{
    // Vec - Vec
    Vec2 vec(5.f, -5.f);
    Vec2 vec2(25.f, -7.f);
    Vec2 vec3 = vec - vec2;
    EXPECT_VEC2_EQf(vec3, -20.f, 2.f);

    // Vec -= Vec
    vec3 -= vec;
    EXPECT_VEC2_EQf(vec3, -25.f, 7.f);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(Vec2, Multiply)
{
    // Vec * Vec
    Vec2 vec(2.f, -3.f);
    Vec2 vec2(5.f, 4.f);
    Vec2 vec3 = vec * vec2;
    EXPECT_VEC2_EQf(vec3, 10.f, -12.f);

    // Vec *= Vec
    vec3 *= vec;
    EXPECT_VEC2_EQf(vec3, 20.f, 36.f);

    // Vec * float
    Vec2 vec4 = vec3 * 0.5f;
    EXPECT_VEC2_EQf(vec4, 10.f, 18.f);

    // Vec *= float
    vec3 *= -2.f;
    EXPECT_VEC2_EQf(vec3, -40.f, -72.f);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(Vec2, Divide)
{
    // Vec / Vec
    Vec2 vec(20.f, -81.f);
    Vec2 vec2(5.f, -3.f);
    Vec2 vec3 = vec / vec2;
    EXPECT_VEC2_EQf(vec3, 4.f, 27.f);

    // Vec /= Vec
    vec3 /= vec2;
    EXPECT_VEC2_EQf(vec3, 0.8f, -9.f);

    // Vec / float
    Vec2 vec4 = vec2 / 2.f;
    EXPECT_VEC2_EQf(vec4, 2.5f, -1.5f);

    // Vec /= float
    vec /= 2.f;
    EXPECT_VEC2_EQf(vec, 10.f, -40.5f);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(Vec2, Normalize)
{
    Vec2 ez(3.f, 4.f);
    EXPECT_FLOAT_EQ(ez.GetLength(), 5.f);
    
    EXPECT_FLOAT_EQ(ez.GetNormalized().GetLength(), 1.f);

    ez.Normalize();
    EXPECT_FLOAT_EQ(ez.GetLength(), 1.f);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(Vec2, ClampLength)
{
    Vec2 ez(3.f, 4.f);
    EXPECT_FLOAT_EQ(ez.GetLength(), 5.f);
    ez.ClampLength(1.f);
    EXPECT_FLOAT_EQ(ez.GetLength(), 1.f);
    ez.ClampLength(0.000000001f);
    EXPECT_FLOAT_EQ(ez.GetLength(), 0.000000001f);
    ez.ClampLength(0.f);
    EXPECT_FLOAT_EQ(ez.GetLength(), 0.f);
    
    Vec2 zero;
    EXPECT_FLOAT_EQ(zero.GetLength(), 0.f);
    zero.ClampLength(1.f);
    EXPECT_FLOAT_EQ(zero.GetLength(), 0.f);
}