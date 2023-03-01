// Bradley Christensen - 2022
#include "gtest/gtest.h"
#include "Game/UnitTests/TestUtils.h"
#include "Engine/Math/AABB2.h"



TEST(AABB2, Construct)
{
    AABB2 box(-2.f, -3.f, 5.f, 10.f);
    EXPECT_VEC2_EQf(box.mins, -2.f, -3.f);
    EXPECT_VEC2_EQf(box.maxs, 5.f, 10.f);
}

TEST(AABB2, Dimensions)
{
    AABB2 box(-2.f, -3.f, 4.f, 9.f);
    EXPECT_FLOAT_EQ(box.GetWidth(), 6.f);
    EXPECT_FLOAT_EQ(box.GetHeight(), 12.f);
    EXPECT_VEC2_EQf(box.GetDimensions(), 6.f, 12.f);
    EXPECT_VEC2_EQf(box.GetHalfDimensions(), 3.f, 6.f);
    EXPECT_FLOAT_EQ(box.GetAspect(), 0.5f);
    EXPECT_VEC2_EQf(box.GetCenter(), 1.f, 3.f);
    EXPECT_VEC2_EQf(box.GetTopLeft(), -2.f, 9.f);
    EXPECT_VEC2_EQf(box.GetBottomRight(), 4.f, -3.f);
}

TEST(AABB2, Translate)
{
    AABB2 box(-2.f, -3.f, 4.f, 9.f);
    Vec2 centerBefore = box.GetCenter();
    Vec2 translation = Vec2(-5.f, 5.f);
    box.Translate(translation);
    EXPECT_TRUE(centerBefore + translation == box.GetCenter());
}

TEST(AABB2, SetCenter)
{
    AABB2 box(-2.f, -3.f, 4.f, 9.f);
    Vec2 dims = box.GetDimensions();
    Vec2 newCenter = Vec2(100.f, 100.f);
    box.SetCenter(newCenter);
    EXPECT_VEC2_EQ(newCenter, box.GetCenter());
    EXPECT_VEC2_EQ(dims, box.GetDimensions());

    Vec2 newDims = Vec2(1.f, 1.f);
    box.SetDimsAboutCenter(newDims);
    EXPECT_VEC2_EQ(newDims, box.GetDimensions());
    
    Vec2 newHalfDims = Vec2(7.f, 7.f);
    box.SetHalfDimsAboutCenter(newHalfDims);
    Vec2 boxDims = box.GetDimensions();
    EXPECT_VEC2_EQ(newHalfDims * 2.f, boxDims);
}

TEST(AABB2, Overlap)
{
    AABB2 box1(-2.f, -3.f, 4.f, 9.f);
    AABB2 box2(-20.f, -15.f, 10.f, 4.f);

    EXPECT_TRUE(box1.IsOverlapping(box2));
    EXPECT_TRUE(box2.IsOverlapping(box1));
    
    AABB2 box3(-2.f, -3.f, 4.f, 9.f);
    AABB2 box4(-40.f, -40.f, -20.f, -20.f);
    
    EXPECT_FALSE(box3.IsOverlapping(box4));
    EXPECT_FALSE(box4.IsOverlapping(box3));
}

