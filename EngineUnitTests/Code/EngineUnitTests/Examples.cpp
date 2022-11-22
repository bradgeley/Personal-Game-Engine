#include "gtest/gtest.h"



TEST(TestCaseName, TestName)
{
  EXPECT_FLOAT_EQ(10000000.f, 10000001.f);   
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}