// Bradley Christensen - 2022
#pragma once


// Is vector equal to 2 floats
#define EXPECT_VEC2_EQf(vec, X, Y) EXPECT_FLOAT_EQ((vec).x, X); EXPECT_FLOAT_EQ((vec).y, Y)

// Is vector equal to another vector
#define EXPECT_VEC2_EQ(vec1, vec2) EXPECT_FLOAT_EQ((vec1).x, (vec2).x); EXPECT_FLOAT_EQ((vec1).y, (vec2).y)

// Is int vector equal to 2 floats
#define EXPECT_INTVEC2_EQf(vec, X, Y) EXPECT_EQ((vec).x, X); EXPECT_EQ((vec).y, Y)

// Is vector equal to another vector
#define EXPECT_INTVEC2_EQ(vec1, vec2) EXPECT_EQ((vec1).x, (vec2).x); EXPECT_EQ((vec1).y, (vec2).y)

// Is vector equal to 3 floats
#define EXPECT_VEC3_EQf(vec, X, Y, Z) EXPECT_FLOAT_EQ((vec).x, X); EXPECT_FLOAT_EQ((vec).y, Y); EXPECT_FLOAT_EQ((vec).z, Z)

// Is vector equal to another vector
#define EXPECT_VEC3_EQ(vec1, vec2) EXPECT_FLOAT_EQ((vec1).x, (vec2).x); EXPECT_FLOAT_EQ((vec1).y, (vec2).y); EXPECT_FLOAT_EQ((vec1).z, (vec2).z)