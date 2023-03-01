// Bradley Christensen - 2022
#include "Engine/Math/IntVec2.h"
#include "gtest/gtest.h"
#include "Game/UnitTests/TestUtils.h"



TEST(IntVec2, Construct)
{
    // Construction
    IntVec2 vec(5, -5);
    EXPECT_INTVEC2_EQf(vec, 5, -5);

    // Copy construction
    IntVec2 vec2(vec);
    EXPECT_INTVEC2_EQf(vec2, 5, -5);

    // operator=
    IntVec2 vec3 = vec;
    EXPECT_INTVEC2_EQf(vec3, 5, -5);

    // negation
    IntVec2 vec4 = -vec3;
    EXPECT_INTVEC2_EQf(vec4, -5, 5);
}

TEST(IntVec2, Statics)
{
    EXPECT_INTVEC2_EQf(IntVec2::ZeroVector, 0, 0);
    EXPECT_INTVEC2_EQf(IntVec2::ZeroToOne, 0, 1);
}

TEST(IntVec2, Add)
{
    // Vec + Vec 
    IntVec2 vec(5, -5);
    IntVec2 vec2(25, -7);
    IntVec2 vec3 = vec + vec2;
    EXPECT_INTVEC2_EQf(vec3, 30, -12);

    // Vec += Vec
    vec3 += vec;
    EXPECT_INTVEC2_EQf(vec3, 35, -17);
}

TEST(IntVec2, Subtract)
{
    // Vec - Vec
    IntVec2 vec(5, -5);
    IntVec2 vec2(25, -7);
    IntVec2 vec3 = vec - vec2;
    EXPECT_INTVEC2_EQf(vec3, -20, 2);

    // Vec -= Vec
    vec3 -= vec;
    EXPECT_INTVEC2_EQf(vec3, -25, 7);
}