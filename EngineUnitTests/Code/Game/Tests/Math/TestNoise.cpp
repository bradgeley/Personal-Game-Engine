// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Math/Noise.h"
#include <gtest/gtest.h>



//----------------------------------------------------------------------------------------------------------------------
namespace TestNoise
{

    //----------------------------------------------------------------------------------------------------------------------
    // Test 1: GetNoiseZeroToOne1D
    //
    TEST(NoiseTests, GetNoiseZeroToOne1D)
    {
        float n1 = GetNoiseZeroToOne1D(0, 0);
        float n2 = GetNoiseZeroToOne1D(100, 0);
        float n3 = GetNoiseZeroToOne1D(0, 1);
        EXPECT_GE(n1, 0.f);
        EXPECT_LE(n1, 1.f);
        EXPECT_GE(n2, 0.f);
        EXPECT_LE(n2, 1.f);
        EXPECT_GE(n3, 0.f);
        EXPECT_LE(n3, 1.f);
        EXPECT_NE(n1, n2);
        EXPECT_NE(n1, n3);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 2: GetNoiseZeroToOne2D
    //
    TEST(NoiseTests, GetNoiseZeroToOne2D)
    {
        float n1 = GetNoiseZeroToOne2D(0, 0, 0);
        float n2 = GetNoiseZeroToOne2D(10, 10, 0);
        float n3 = GetNoiseZeroToOne2D(0, 0, 1);
        EXPECT_GE(n1, 0.f);
        EXPECT_LE(n1, 1.f);
        EXPECT_GE(n2, 0.f);
        EXPECT_LE(n2, 1.f);
        EXPECT_GE(n3, 0.f);
        EXPECT_LE(n3, 1.f);
        EXPECT_NE(n1, n2);
        EXPECT_NE(n1, n3);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 4: GetFractalNoise1D
    //
    TEST(NoiseTests, GetFractalNoise1D)
    {
        float n1 = GetFractalNoise1D(0.f, 1.f, 3, 0.5f, 2.f, true, 0);
        float n2 = GetFractalNoise1D(10.f, 1.f, 3, 0.5f, 2.f, true, 0);
        float n3 = GetFractalNoise1D(0.f, 1.f, 3, 0.5f, 2.f, true, 1);
        EXPECT_GE(n1, -1.f);
        EXPECT_LE(n1, 1.f);
        EXPECT_GE(n2, -1.f);
        EXPECT_LE(n2, 1.f);
        EXPECT_GE(n3, -1.f);
        EXPECT_LE(n3, 1.f);
        EXPECT_NE(n1, n2);
        EXPECT_NE(n1, n3);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 5: GetFractalNoise2D
    //
    TEST(NoiseTests, GetFractalNoise2D)
    {
        float n1 = GetFractalNoise2D(0.f, 0.f, 1.f, 3, 0.5f, 2.f, true, 0);
        float n2 = GetFractalNoise2D(5.f, 5.f, 1.f, 3, 0.5f, 2.f, true, 0);
        float n3 = GetFractalNoise2D(0.f, 0.f, 1.f, 3, 0.5f, 2.f, true, 1);
        EXPECT_GE(n1, -1.f);
        EXPECT_LE(n1, 1.f);
        EXPECT_GE(n2, -1.f);
        EXPECT_LE(n2, 1.f);
        EXPECT_GE(n3, -1.f);
        EXPECT_LE(n3, 1.f);
        EXPECT_NE(n1, n2);
        EXPECT_NE(n1, n3);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 7: GetPerlinNoise1D
    //
    TEST(NoiseTests, GetPerlinNoise1D)
    {
        float n1 = GetPerlinNoise1D(0.5f, 10.f, 5, 0.5f, 2.f, true, 0);
        float n2 = GetPerlinNoise1D(10.75f, 7.f, 4, 0.5f, 2.f, true, 1);
        float n3 = GetPerlinNoise1D(0.25f, 6.f, 3, 0.5f, 2.f, true, 2);
        EXPECT_GE(n1, -1.f);
        EXPECT_LE(n1, 1.f);
        EXPECT_GE(n2, -1.f);
        EXPECT_LE(n2, 1.f);
        EXPECT_GE(n3, -1.f);
        EXPECT_LE(n3, 1.f);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 9: GetPerlinNoise1D_FullRange
    //
    TEST(NoiseTests, GetPerlinNoise1D_FullRange)
    {
        float minValue = 5.f;
        float maxValue = -5.f;
        float scale = 1.123843f;
        int octaves = 3;
        float octavePersistence = 0.5f;
        float octaveScale = 2.f;
        bool normalize = true;
        int seed = 100;

        float x = 8745.47234;
        for (int i = 0; i < 100'000; ++i)
        {
            x += 0.001815739;
            float n = GetPerlinNoise1D(x, scale, octaves, octavePersistence, octaveScale, normalize, seed);
            if (n < minValue) minValue = n;
            if (n > maxValue) maxValue = n;
        }
        // Ensure at least one value is close to -1 and one is close to 1
        EXPECT_LT(minValue, -0.95f);
        EXPECT_GT(maxValue, 0.95f);
    }



}
