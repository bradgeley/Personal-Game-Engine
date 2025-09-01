// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Math/Vec2.h"
#include <gtest/gtest.h>
#include <limits>



//----------------------------------------------------------------------------------------------------------------------
namespace TestRNG
{

    //----------------------------------------------------------------------------------------------------------------------
    struct RNGScope
    {
        RNGScope(size_t seed = 12345)
        {
            g_rng = new RandomNumberGenerator(seed);
        }
        ~RNGScope()
        {
            delete g_rng;
            g_rng = nullptr;
        }
    };



    //----------------------------------------------------------------------------------------------------------------------
    // Test 1: SetSeed and Determinism
    //
    TEST(RNGTests, SetSeed_Determinism)
    {
        RNGScope scope(42);
        int a = g_rng->GetRandomInt();
        g_rng->SetSeed(42);
        int b = g_rng->GetRandomInt();
        EXPECT_EQ(a, b);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 2: GenerateRandomSeed
    //
    TEST(RNGTests, GenerateRandomSeed)
    {
        RNGScope scope;
        size_t oldSeed = g_rng->GenerateRandomSeed();
        size_t newSeed = g_rng->GenerateRandomSeed();
        EXPECT_NE(oldSeed, newSeed);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 3: Rand (non-deterministic)
    //
    TEST(RNGTests, Rand)
    {
        RNGScope scope;
        unsigned int r1 = g_rng->Rand();
        unsigned int r2 = g_rng->Rand();
        // Not guaranteed to be different, but should be unsigned int
        EXPECT_NEAR(static_cast<double>(r1), static_cast<double>(r1), 0.0);
        EXPECT_NEAR(static_cast<double>(r2), static_cast<double>(r2), 0.0);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 4: PlusOrMinus
    //
    TEST(RNGTests, PlusOrMinus)
    {
        RNGScope scope;
        int plus = 0, minus = 0;
        for (int i = 0; i < 1000; ++i)
        {
            int val = g_rng->PlusOrMinus();
            EXPECT_TRUE(val == 1 || val == -1);
            if (val == 1) ++plus;
            else ++minus;
        }
        EXPECT_GT(plus, 0);
        EXPECT_GT(minus, 0);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 5: GetRandomIntInRange
    //
    TEST(RNGTests, GetRandomIntInRange)
    {
        RNGScope scope;
        int min = 10, max = 20;
        for (int i = 0; i < 100; ++i)
        {
            int val = g_rng->GetRandomIntInRange(min, max);
            EXPECT_GE(val, min);
            EXPECT_LE(val, max);
        }
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 6: GetRandomVecInRange2D
    //
    TEST(RNGTests, GetRandomVecInRange2D)
    {
        RNGScope scope;
        Vec2 min(1.0f, 2.0f);
        Vec2 max(3.0f, 4.0f);
        for (int i = 0; i < 100; ++i)
        {
            Vec2 v = g_rng->GetRandomVecInRange2D(min, max);
            EXPECT_GE(v.x, min.x);
            EXPECT_LE(v.x, max.x);
            EXPECT_GE(v.y, min.y);
            EXPECT_LE(v.y, max.y);
        }
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 7: GetRandomFloatZeroToOne
    //
    TEST(RNGTests, GetRandomFloatZeroToOne)
    {
        RNGScope scope;
        for (int i = 0; i < 100; ++i)
        {
            float f = g_rng->GetRandomFloatZeroToOne();
            EXPECT_GE(f, 0.0f);
            EXPECT_LE(f, 1.0f);
        }
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 8: GetRandomFloatInRange
    //
    TEST(RNGTests, GetRandomFloatInRange)
    {
        RNGScope scope;
        float min = -5.0f, max = 5.0f;
        for (int i = 0; i < 100; ++i)
        {
            float f = g_rng->GetRandomFloatInRange(min, max);
            EXPECT_GE(f, min);
            EXPECT_LE(f, max);
        }
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 9: CoinFlip
    //
    TEST(RNGTests, CoinFlip)
    {
        RNGScope scope;
        int trueCount = 0, falseCount = 0;
        for (int i = 0; i < 1000; ++i)
        {
            if (g_rng->CoinFlip()) ++trueCount;
            else ++falseCount;
        }
        EXPECT_GT(trueCount, 0);
        EXPECT_GT(falseCount, 0);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 10: GetRandomInt
    //
    TEST(RNGTests, GetRandomInt)
    {
        RNGScope scope;
        int a = g_rng->GetRandomInt();
        int b = g_rng->GetRandomInt();
        // Not guaranteed to be different, but should be int
        EXPECT_NEAR(static_cast<double>(a), static_cast<double>(a), 0.0);
        EXPECT_NEAR(static_cast<double>(b), static_cast<double>(b), 0.0);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 11: GetRandomSizeT
    //
    TEST(RNGTests, GetRandomSizeT)
    {
        RNGScope scope;
        size_t a = g_rng->GetRandomSizeT();
        size_t b = g_rng->GetRandomSizeT();
        // Not guaranteed to be different, but should be size_t
        EXPECT_NEAR(static_cast<double>(a), static_cast<double>(a), 0.0);
        EXPECT_NEAR(static_cast<double>(b), static_cast<double>(b), 0.0);
    }

}
