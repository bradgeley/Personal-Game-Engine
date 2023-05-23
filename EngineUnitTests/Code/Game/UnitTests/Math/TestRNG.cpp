// Bradley Christensen - 2023
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Math/StatisticsUtils.h"
#include "gtest/gtest.h"
#include "Game/UnitTests/TestUtils.h"



int RNG_TEST_NUM_ITERATIONS = 100000;



//----------------------------------------------------------------------------------------------------------------------
TEST(RNG, CreateDestroy)
{
    g_rng = new RandomNumberGenerator();
    EXPECT_TRUE(g_rng != nullptr);
    delete g_rng;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(RNG, Rand)
{
    g_rng = new RandomNumberGenerator();
    for (int t=0; t<RNG_TEST_NUM_ITERATIONS; ++t)
    {
        int i = g_rng->Rand();
        EXPECT_GE(i, 0);
        EXPECT_LE(i, RAND_MAX);
    }
    delete g_rng;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(RNG, SeededRand)
{
    g_rng = new RandomNumberGenerator();
    for (int t=0; t<RNG_TEST_NUM_ITERATIONS; ++t)
    {
        g_rng->SetSeed(t);
        int a = g_rng->Rand();
        g_rng->SetSeed(t);
        int b = g_rng->Rand();
        EXPECT_EQ(a, b);
    }
    delete g_rng;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(RNG, GetRandomIntInRange)
{
    g_rng = new RandomNumberGenerator();
    int range = 100;
    std::vector<int> results;
    for (int t=0; t<RNG_TEST_NUM_ITERATIONS; ++t)
    {
        int v = g_rng->GetRandomIntInRange(-range, range);
        results.emplace_back(v);
        
        EXPECT_GE(v, -range);
        EXPECT_LE(v, range);
    }

    if (results.size() > 0)
    {
        double average = StatisticsUtils::GetAverage(results.data(), results.size());
        int max = *StatisticsUtils::GetMax(results.data(), results.size());
        int min = *StatisticsUtils::GetMin(results.data(), results.size());
        printf("Average val (range=%i): %f\n", range, average);
        printf("Max val (range=%i): %i\n", range, max);
        printf("Min val (range=%i): %i\n", range, min);
    }
    
    delete g_rng;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(RNG, GetRandomFloatZeroToOne)
{
    g_rng = new RandomNumberGenerator();
    std::vector<float> results;
    for (int t=0; t<RNG_TEST_NUM_ITERATIONS; ++t)
    {
        float v = g_rng->GetRandomFloatZeroToOne();
        results.emplace_back(v);

        EXPECT_GE(v, 0.f);
        EXPECT_LE(v, 1.f);
    }
    
    if (results.size() > 0)
    {
        double average = StatisticsUtils::GetAverage(results.data(), results.size());
        float max = *StatisticsUtils::GetMax(results.data(), results.size());
        float min = *StatisticsUtils::GetMin(results.data(), results.size());
        printf("Average val: %f\n", average);
        printf("Max val: %f\n", max);
        printf("Min val: %f\n", min);
    }
    
    delete g_rng;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(RNG, GetRandomFloatInRange)
{
    g_rng = new RandomNumberGenerator();
    float range = 100.f;
    std::vector<float> results;
    for (int t=0; t<RNG_TEST_NUM_ITERATIONS; ++t)
    {
        float v = g_rng->GetRandomFloatInRange(-range, range);
        results.emplace_back(v);

        EXPECT_GE(v, -range);
        EXPECT_LE(v, range);
    }
    
    if (results.size() > 0)
    {
        double average = StatisticsUtils::GetAverage(results.data(), results.size());
        float max = *StatisticsUtils::GetMax(results.data(), results.size());
        float min = *StatisticsUtils::GetMin(results.data(), results.size());
        printf("Average val (range=%f): %f\n", range, average);
        printf("Max val (range=%f): %f\n", range, max);
        printf("Min val (range=%f): %f\n", range, min);
    }
    
    delete g_rng;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(RNG, CoinFlip)
{
    g_rng = new RandomNumberGenerator();
    std::vector<bool> results;
    for (int t=0; t<RNG_TEST_NUM_ITERATIONS; ++t)
    {
        bool v = g_rng->CoinFlip(0.5f);
        results.emplace_back(v);
    }
    
    if ((int) results.size() > 0)
    {
        int numTrue = 0;
        int numFalse = 0;
        for (auto const& val : results)
        {
            if (val) numTrue++;
            else numFalse++;
        }
        printf("Trues: %i, Falses: %i\n", numTrue, numFalse);
    }
    
    delete g_rng;
}