// Bradley Christensen - 2022
#pragma once


// Is vector equal to 2 floats
#define EXPECT_VEC2_EQf(vec1, X, Y) EXPECT_FLOAT_EQ((vec1).x, X); EXPECT_FLOAT_EQ((vec1).y, Y)

// Is vector equal to another vector
#define EXPECT_VEC2_EQ(vec1, vec2) EXPECT_FLOAT_EQ((vec1).x, (vec2).x); EXPECT_FLOAT_EQ((vec1).y, (vec2).y)
