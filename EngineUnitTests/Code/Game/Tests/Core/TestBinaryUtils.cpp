// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Core/BinaryUtils.h"
#include <gtest/gtest.h>
#include <vector>
#include <cstdint>
#include <cstring>

//----------------------------------------------------------------------------------------------------------------------
// BinaryUtils Unit Tests
//
namespace TestBinaryUtils
{

    using namespace BinaryUtils;

    // FirstSetBit(size_t mask)
    TEST(BinaryUtils, FirstSetBit)
    {
        // Case 1: No bits set
        EXPECT_EQ(FirstSetBit(0), -1);

        // Case 2: First bit set
        EXPECT_EQ(FirstSetBit(0x1), 0);

        // Case 3: Second bit set
        EXPECT_EQ(FirstSetBit(0x2), 1);

        // Case 4: Multiple bits set, first at index 3
        EXPECT_EQ(FirstSetBit(0x8 | 0x10), 3);

        // Case 5: All bits set
        EXPECT_EQ(FirstSetBit(static_cast<size_t>(-1)), 0);
    }

    // FirstSetBit(size_t mask, int firstValidIndex)
    TEST(BinaryUtils, FirstSetBitWithFirstValidIndex)
    {
        // Case 1: No bits set
        EXPECT_EQ(FirstSetBit(0, 2), -1);

        // Case 2: First valid bit set
        EXPECT_EQ(FirstSetBit(0x4, 2), 2);

        // Case 3: Bit set before firstValidIndex
        EXPECT_EQ(FirstSetBit(0x2, 2), -1);

        // Case 4: Multiple bits set, first valid at index 5
        EXPECT_EQ(FirstSetBit(0x20 | 0x40, 5), 5);

        // Case 5: All bits set, first valid at index 10
        EXPECT_EQ(FirstSetBit(static_cast<size_t>(-1), 10), 10);
    }

    // FirstUnsetBit(size_t mask)
    TEST(BinaryUtils, FirstUnsetBit)
    {
        // Case 1: All bits set
        EXPECT_EQ(FirstUnsetBit(static_cast<size_t>(-1)), -1);

        // Case 2: No bits set
        EXPECT_EQ(FirstUnsetBit(0), 0);

        // Case 3: Only first bit set
        EXPECT_EQ(FirstUnsetBit(0x1), 1);

        // Case 4: Only second bit set
        EXPECT_EQ(FirstUnsetBit(0x2), 0);

        // Case 5: High bit set, first unset at 0
        EXPECT_EQ(FirstUnsetBit(static_cast<size_t>(1) << (sizeof(size_t) * 8 - 1)), 0);
    }

    // FirstUnsetBit(size_t mask, int firstValidIndex)
    TEST(BinaryUtils, FirstUnsetBitWithFirstValidIndex)
    {
        // Case 1: All bits set
        EXPECT_EQ(FirstUnsetBit(static_cast<size_t>(-1), 5), -1);

        // Case 2: No bits set, first valid at 3
        EXPECT_EQ(FirstUnsetBit(0, 3), 3);

        // Case 3: Bits set before firstValidIndex
        EXPECT_EQ(FirstUnsetBit(0x7, 3), 3);

        // Case 4: Bit at firstValidIndex set, next unset
        EXPECT_EQ(FirstUnsetBit(0x8, 3), 4);

        // Case 5: All bits set except one after firstValidIndex
        size_t mask = static_cast<size_t>(-1) & ~(static_cast<size_t>(1) << 10);
        EXPECT_EQ(FirstUnsetBit(mask, 5), 10);
    }

}