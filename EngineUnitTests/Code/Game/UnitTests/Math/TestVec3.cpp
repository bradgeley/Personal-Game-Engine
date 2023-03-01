// Bradley Christensen - 2022
#include "gtest/gtest.h"
#include "Game/UnitTests/TestUtils.h"
#include "Engine/Math/Vec3.h"


TEST(Vec3, Construct)
{
    // Construction
    Vec3 vec(5.f, -5.f, 2.f);
    EXPECT_VEC3_EQf(vec, 5, -5.f, 2.f);

    // Copy construction
    Vec3 vec2(vec);
    EXPECT_VEC3_EQf(vec2, 5, -5.f, 2.f);

    // operator=
    Vec3 vec3 = vec;
    EXPECT_VEC3_EQf(vec3, 5, -5.f, 2.f);

    // negation
    Vec3 vec4 = -vec3;
    EXPECT_VEC3_EQf(vec4, -5, 5.f, -2.f);
}

TEST(Vec3, Equality)
{
    // Construction
    Vec3 vec(5.f, -5.f, 2.f);
    Vec3 vec2(5.f, -5.f, 2.f);
    EXPECT_TRUE(vec == vec2);
}

TEST(Vec3, Statics)
{
    EXPECT_VEC3_EQf(Vec3::ZeroVector, 0.f, 0.f, 0.f);
}

TEST(Vec3, Add)
{
    // Vec + Vec 
    Vec3 vec(5.f, -5.f, 2.f);
    Vec3 vec2(25.f, -7.f, 2.f);
    Vec3 vec3 = vec + vec2;
    EXPECT_VEC3_EQf(vec3, 30.f, -12.f, 4.f);

    // Vec += Vec
    vec3 += vec;
    EXPECT_VEC3_EQf(vec3, 35.f, -17.f, 6.f);
}

TEST(Vec3, Subtract)
{
    // Vec - Vec
    Vec3 vec(5.f, -5.f, 2.f);
    Vec3 vec2(25.f, -7.f, 2.f);
    Vec3 vec3 = vec - vec2;
    EXPECT_VEC3_EQf(vec3, -20.f, 2.f, 0.f);

    // Vec -= Vec
    vec3 -= vec;
    EXPECT_VEC3_EQf(vec3, -25.f, 7.f, -2.f);
}

TEST(Vec3, Multiply)
{
    // Vec * Vec
    Vec3 vec(2.f, -3.f, 2.f);
    Vec3 vec2(5.f, 4.f, 2.f);
    Vec3 vec3 = vec * vec2;
    EXPECT_VEC3_EQf(vec3, 10.f, -12.f, 4.f);

    // Vec *= Vec
    vec3 *= vec;
    EXPECT_VEC3_EQf(vec3, 20.f, 36.f, 8.f);

    // Vec * float
    Vec3 vec4 = vec3 * 0.5f;
    EXPECT_VEC3_EQf(vec4, 10.f, 18.f, 4.f);

    // Vec *= float
    vec3 *= -2.f;
    EXPECT_VEC3_EQf(vec3, -40.f, -72.f, -16.f);
}

TEST(Vec3, Divide)
{
    // Vec / Vec
    Vec3 vec(20.f, -81.f, 2.f);
    Vec3 vec2(5.f, -3.f, 2.f);
    Vec3 vec3 = vec / vec2;
    EXPECT_VEC3_EQf(vec3, 4.f, 27.f, 1.f);

    // Vec /= Vec
    vec3 /= vec2;
    EXPECT_VEC3_EQf(vec3, 0.8f, -9.f, 0.5f);

    // Vec / float
    Vec3 vec4 = vec2 / 2.f;
    EXPECT_VEC3_EQf(vec4, 2.5f, -1.5f, 1.f);

    // Vec /= float
    vec /= 2.f;
    EXPECT_VEC3_EQf(vec, 10.f, -40.5f, 1.f);
}

TEST(Vec3, Normalize)
{
    Vec3 vec(2.f, 3.f, 6.f);
    EXPECT_FLOAT_EQ(vec.GetLength(), 7.f);
    
    EXPECT_FLOAT_EQ(vec.GetNormalized().GetLength(), 1.f);

    vec.Normalize();
    EXPECT_FLOAT_EQ(vec.GetLength(), 1.f);
}