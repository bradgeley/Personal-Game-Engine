// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Math/StatisticsUtils.h"
#include <gtest/gtest.h>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
// StatisticsUtils Unit Tests
//
namespace TestStatsUtils
{
    using namespace StatisticsUtils;



    //------------------------------------------------------------------------------------------------------------------
    // Test: GetAverage
    //
    TEST(StatisticsUtils, GetAverage_Int)
    {
        int arr[] = { 1, 2, 3, 4, 5 };
        EXPECT_DOUBLE_EQ(GetAverage(arr, 5), 3.0);

        int arr2[] = { -10, 0, 10 };
        EXPECT_DOUBLE_EQ(GetAverage(arr2, 3), 0.0);

        int arr3[] = { 42 };
        EXPECT_DOUBLE_EQ(GetAverage(arr3, 1), 42.0);

        int arr4[] = { 0 };
        EXPECT_DOUBLE_EQ(GetAverage(arr4, 0), 0.0);
    }

    TEST(StatisticsUtils, GetAverage_Double)
    {
        double arr[] = { 1.5, 2.5, 3.5 };
        EXPECT_DOUBLE_EQ(GetAverage(arr, 3), 2.5);

        double arr2[] = { -1.0, 1.0 };
        EXPECT_DOUBLE_EQ(GetAverage(arr2, 2), 0.0);

        double arr3[] = { 7.7 };
        EXPECT_DOUBLE_EQ(GetAverage(arr3, 1), 7.7);

        double arr4[] = { 0.0 };
        EXPECT_DOUBLE_EQ(GetAverage(arr4, 0), 0.0);
    }



    //------------------------------------------------------------------------------------------------------------------
    // Test: GetMin
    //
    TEST(StatisticsUtils, GetMin_Int)
    {
        int arr[] = { 5, 2, 8, 1, 3 };
        int* minPtr = GetMin(arr, 5);
        ASSERT_NE(minPtr, nullptr);
        EXPECT_EQ(*minPtr, 1);

        int arr2[] = { 42 };
        int* minPtr2 = GetMin(arr2, 1);
        ASSERT_NE(minPtr2, nullptr);
        EXPECT_EQ(*minPtr2, 42);

        int arr3[] = { 0 };
        int* minPtr3 = GetMin(arr3, 0);
        EXPECT_EQ(minPtr3, nullptr);
    }

    TEST(StatisticsUtils, GetMin_Double)
    {
        double arr[] = { 3.5, -2.1, 7.8, 0.0 };
        double* minPtr = GetMin(arr, 4);
        ASSERT_NE(minPtr, nullptr);
        EXPECT_DOUBLE_EQ(*minPtr, -2.1);

        double arr2[] = { 9.9 };
        double* minPtr2 = GetMin(arr2, 1);
        ASSERT_NE(minPtr2, nullptr);
        EXPECT_DOUBLE_EQ(*minPtr2, 9.9);

        double arr3[] = { 0.0 };
        double* minPtr3 = GetMin(arr3, 0);
        EXPECT_EQ(minPtr3, nullptr);
    }



    //------------------------------------------------------------------------------------------------------------------
    // Test: GetMax
    //
    TEST(StatisticsUtils, GetMax_Int)
    {
        int arr[] = { 5, 2, 8, 1, 3 };
        int* maxPtr = GetMax(arr, 5);
        ASSERT_NE(maxPtr, nullptr);
        EXPECT_EQ(*maxPtr, 8);

        int arr2[] = { 42 };
        int* maxPtr2 = GetMax(arr2, 1);
        ASSERT_NE(maxPtr2, nullptr);
        EXPECT_EQ(*maxPtr2, 42);

        int arr3[] = { 0 };
        int* maxPtr3 = GetMax(arr3, 0);
        EXPECT_EQ(maxPtr3, nullptr);
    }

    TEST(StatisticsUtils, GetMax_Double)
    {
        double arr[] = { 3.5, -2.1, 7.8, 0.0 };
        double* maxPtr = GetMax(arr, 4);
        ASSERT_NE(maxPtr, nullptr);
        EXPECT_DOUBLE_EQ(*maxPtr, 7.8);

        double arr2[] = { -9.9 };
        double* maxPtr2 = GetMax(arr2, 1);
        ASSERT_NE(maxPtr2, nullptr);
        EXPECT_DOUBLE_EQ(*maxPtr2, -9.9);

        double arr3[] = { 0.0 };
        double* maxPtr3 = GetMax(arr3, 0);
        EXPECT_EQ(maxPtr3, nullptr);
    }

}
