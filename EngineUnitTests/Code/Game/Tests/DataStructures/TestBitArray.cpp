// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/DataStructures/BitArray.h"
#include <gtest/gtest.h>
#include <random>

namespace TestBitArray
{

    // Helper for basic construction and size
    TEST(BitArray, ConstructionAndSize)
    {
        BitArray<16> arr;
        EXPECT_EQ(arr.Size(), 16);
    }

    // Test Set, Unset, Get, operator[]
    TEST(BitArray, SetUnsetGetOperator)
    {
        BitArray<8> arr;
        for (int i = 0; i < arr.Size(); ++i)
        {
            EXPECT_FALSE(arr.Get(i));
            EXPECT_FALSE(arr[i]);
        }
        arr.Set(3);
        EXPECT_TRUE(arr.Get(3));
        EXPECT_TRUE(arr[3]);
        arr.Unset(3);
        EXPECT_FALSE(arr.Get(3));
        EXPECT_FALSE(arr[3]);
    }

    // Test SetAll
    TEST(BitArray, SetAll)
    {
        BitArray<10> arr;
        arr.SetAll(true);
        for (int i = 0; i < arr.Size(); ++i)
        {
            EXPECT_TRUE(arr.Get(i));
        }
        arr.SetAll(false);
        for (int i = 0; i < arr.Size(); ++i)
        {
            EXPECT_FALSE(arr.Get(i));
        }
    }

    // Test Flip
    TEST(BitArray, Flip)
    {
        BitArray<5> arr;
        EXPECT_TRUE(arr.Flip(2));
        EXPECT_TRUE(arr.Get(2));
        EXPECT_FALSE(arr.Flip(2));
        EXPECT_FALSE(arr.Get(2));
    }

    // Test GetFirstUnsetIndex and GetFirstSetIndex
    TEST(BitArray, GetFirstUnsetAndSetIndex)
    {
        BitArray<8> arr(true);

		int unsetIdx = arr.GetFirstUnsetIndex();
        EXPECT_EQ(unsetIdx, -1);
        arr.Unset(5);

        unsetIdx = arr.GetFirstUnsetIndex();
        EXPECT_EQ(unsetIdx, 5);

        arr.SetAll(false);
        int firstSetIdx = arr.GetFirstSetIndex();
        EXPECT_EQ(firstSetIdx, -1);

        arr.Set(4);
        firstSetIdx = arr.GetFirstSetIndex();
        EXPECT_EQ(firstSetIdx, 4);
    }

    // Test GetNextUnsetIndex and SetNextUnsetIndex
    TEST(BitArray, GetNextUnsetAndSetNextUnsetIndex)
    {
        BitArray<8> arr(true);
        arr.Unset(2);
        arr.Unset(5);
        EXPECT_EQ(arr.GetNextUnsetIndex(0), 2);
        EXPECT_EQ(arr.GetNextUnsetIndex(3), 5);
        EXPECT_EQ(arr.GetNextUnsetIndex(6), -1);
        int idx = arr.SetNextUnsetIndex(0);
        EXPECT_EQ(idx, 2);
        EXPECT_TRUE(arr.Get(2));
        idx = arr.SetNextUnsetIndex(0);
        EXPECT_EQ(idx, 5);
        EXPECT_TRUE(arr.Get(5));
        idx = arr.SetNextUnsetIndex(0);
        EXPECT_EQ(idx, -1);
    }

    // Test GetNextSetIndex
    TEST(BitArray, GetNextSetIndex)
    {
        BitArray<8> arr;
        arr.Set(1);
        arr.Set(6);
        EXPECT_EQ(arr.GetNextSetIndex(0), 1);
        EXPECT_EQ(arr.GetNextSetIndex(2), 6);
        EXPECT_EQ(arr.GetNextSetIndex(7), -1);
    }

    // Test CountSetBits
    TEST(BitArray, CountSetBits)
    {
        BitArray<16> arr;
        EXPECT_EQ(arr.CountSetBits(), 0);
        arr.Set(0);
        arr.Set(5);
        arr.Set(15);
        EXPECT_EQ(arr.CountSetBits(), 3);
        arr.SetAll(true);
        EXPECT_EQ(arr.CountSetBits(), 16);
        arr.SetAll(false);
        EXPECT_EQ(arr.CountSetBits(), 0);
    }

    // Test trailing bits are handled correctly (size not multiple of 8/64)
    // Test trailing bits are handled correctly (size not multiple of 8/64)
    TEST(BitArray, TrailingBits)
    {
        BitArray<70> arr;
        arr.SetAll(true);
        EXPECT_EQ(arr.CountSetBits(), 70);
        arr.SetAll(false);
        EXPECT_EQ(arr.CountSetBits(), 0);

        // Set first and last bits, check
        arr.Set(0);
        arr.Set(69);
        EXPECT_TRUE(arr.Get(0));
        EXPECT_TRUE(arr.Get(69));
        EXPECT_EQ(arr.CountSetBits(), 2);

        // Unset first, check only last is set
        arr.Unset(0);
        EXPECT_FALSE(arr.Get(0));
        EXPECT_TRUE(arr.Get(69));
        EXPECT_EQ(arr.CountSetBits(), 1);

        // Unset last, check none set
        arr.Unset(69);
        EXPECT_FALSE(arr.Get(69));
        EXPECT_EQ(arr.CountSetBits(), 0);

        // Set all except last, check
        arr.SetAll(true);
        arr.Unset(69);
        EXPECT_EQ(arr.CountSetBits(), 69);
        EXPECT_FALSE(arr.Get(69));
        EXPECT_TRUE(arr.Get(68));

        // Set only the last bit, check GetFirstSetIndex and GetFirstUnsetIndex
        arr.SetAll(false);
        arr.Set(69);
        EXPECT_EQ(arr.GetFirstSetIndex(), 69);
        EXPECT_EQ(arr.GetFirstUnsetIndex(), 0);
        arr.Set(0);
        EXPECT_EQ(arr.GetFirstSetIndex(), 0);
        arr.Unset(0);
        EXPECT_EQ(arr.GetFirstSetIndex(), 69);
    }


    // Stress tests for BitArray


    // Stress test: Set and unset all bits in a large array
    TEST(BitArray, StressSetUnsetAll)
    {
        constexpr int N = 10000;
        BitArray<N> arr;

        // Set all bits one by one
        for (int i = 0; i < N; ++i)
        {
            arr.Set(i);
            EXPECT_TRUE(arr.Get(i));
        }
        EXPECT_EQ(arr.CountSetBits(), N);

        // Unset all bits one by one
        for (int i = 0; i < N; ++i)
        {
            arr.Unset(i);
            EXPECT_FALSE(arr.Get(i));
        }
        EXPECT_EQ(arr.CountSetBits(), 0);
    }

    // Stress test: Randomly set and unset bits
    TEST(BitArray, StressRandomSetUnset)
    {
        constexpr int N = 10000;
        BitArray<N> arr;
        std::mt19937 rng(12345);
        std::uniform_int_distribution<int> dist(0, N - 1);

        // Randomly set 5000 bits
        std::vector<bool> ref(N, false);
        for (int i = 0; i < 5000; ++i)
        {
            int idx = dist(rng);
            arr.Set(idx);
            ref[idx] = true;
        }
        // Check correctness
        for (int i = 0; i < N; ++i)
        {
            EXPECT_EQ(arr.Get(i), ref[i]);
        }

        // Randomly unset 3000 bits
        for (int i = 0; i < 3000; ++i)
        {
            int idx = dist(rng);
            arr.Unset(idx);
            ref[idx] = false;
        }
        // Check correctness
        for (int i = 0; i < N; ++i)
        {
            EXPECT_EQ(arr.Get(i), ref[i]);
        }
    }

    // Stress test: Flip bits randomly
    TEST(BitArray, StressRandomFlip)
    {
        constexpr int N = 10000;
        BitArray<N> arr;
        std::mt19937 rng(54321);
        std::uniform_int_distribution<int> dist(0, N - 1);

        std::vector<bool> ref(N, false);
        for (int i = 0; i < 10000; ++i)
        {
            int idx = dist(rng);
            bool newVal = arr.Flip(idx);
            ref[idx] = !ref[idx];
            EXPECT_EQ(newVal, ref[idx]);
        }
        // Check correctness
        for (int i = 0; i < N; ++i)
        {
            EXPECT_EQ(arr.Get(i), ref[i]);
        }
    }

    // Stress test: SetNextUnsetIndex and GetNextUnsetIndex
    TEST(BitArray, StressSetNextUnsetIndex)
    {
        constexpr int N = 75;
        BitArray<N> arr;
        arr.SetAll(false);

        // Set all bits using SetNextUnsetIndex
        int count = 0;
        int idx = arr.SetNextUnsetIndex(0);
        while (idx != -1)
        {
            ++count;
            idx = arr.SetNextUnsetIndex(0);
        }
        EXPECT_EQ(count, N);
        EXPECT_EQ(arr.CountSetBits(), N);

        // Unset all bits using Unset
        for (int i = 0; i < N; ++i)
        {
            arr.Unset(i);
        }
        EXPECT_EQ(arr.CountSetBits(), 0);
    }
}
