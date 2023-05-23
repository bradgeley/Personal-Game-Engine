// Bradley Christensen - 2023
#include "Engine/Math/StatisticsUtils.h"
#include "gtest/gtest.h"
#include "Game/UnitTests/TestUtils.h"



int STATISTICS_TEST_NUM_ITERATIONS = 100000;



//----------------------------------------------------------------------------------------------------------------------
TEST(Statistics, Average)
{
    std::vector ints = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    double averageInt = StatisticsUtils::GetAverage(ints.data(), ints.size());
    EXPECT_DOUBLE_EQ(averageInt, 5);

    
    std::vector floats = { -0.f, -1.f, -2.f, -3.f, -4.f, -5.f, -6.f, -7.f, -8.f, -9.f, -10.f };
    double averageFloat = StatisticsUtils::GetAverage(floats.data(), ints.size());
    EXPECT_DOUBLE_EQ(averageFloat, -5.f);
}



//----------------------------------------------------------------------------------------------------------------------
TEST(Statistics, MinMax)
{
    std::vector ints = { 6, 1, 2, 3, 0, 5, 6, 7, 8, 9, 10 };
    int minInt = *StatisticsUtils::GetMin(ints.data(), ints.size());
    int maxInt = *StatisticsUtils::GetMax(ints.data(), ints.size());
    EXPECT_EQ(minInt, 0);
    EXPECT_EQ(maxInt, 10);
    
    std::vector floats = { -0.f, -1.f, -2.f, -3.f, -4.f, -5.f, -6.f, -7.f, -8.f, -9.f, -10.f };
    float minFloat = *StatisticsUtils::GetMin(floats.data(), ints.size());
    float maxFloat = *StatisticsUtils::GetMax(floats.data(), ints.size());
    EXPECT_FLOAT_EQ(minFloat, -10.f);
    EXPECT_FLOAT_EQ(maxFloat, 0.f);
}