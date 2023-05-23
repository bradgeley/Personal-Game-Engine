// Bradley Christensen - 2022
#include "gtest/gtest.h"
#include "Engine/DataStructures/ThreadSafeQueue.h"
#include "Engine/DataStructures/BitArray.h"



//----------------------------------------------------------------------------------------------------------------------
TEST(DataStructures, ThreadSafeQueue)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
template<unsigned int t_size>
void BitArrayUnitTests(BitArray<t_size>& testBitArray);



//----------------------------------------------------------------------------------------------------------------------
TEST(DataStructures, BitArray10)
{
    BitArray<10> b;
    BitArrayUnitTests(b);
}



TEST(DataStructures, BitArray64)
{
    BitArray<64> b;
    BitArrayUnitTests(b);
}



TEST(DataStructures, BitArray128)
{
    BitArray<128> b;
    BitArrayUnitTests(b);
}



TEST(DataStructures, BitArray100)
{
    BitArray<100> b;
    BitArrayUnitTests(b);
}



TEST(DataStructures, BitArray1000000)
{
    BitArray<1'000'000> b;
    BitArrayUnitTests(b);
}



template<unsigned int t_size>
void BitArrayUnitTests(BitArray<t_size>& b)
{
    // Set/Get
    for (int i = 0; i < b.Size(); ++i)
    {
        b.Set(i);
        EXPECT_TRUE(b.Get(i));
        EXPECT_TRUE(b[i]);
    }

    // Unset/Get
    for (int i = 0; i < b.Size(); ++i)
    {
        b.Unset(i);
        EXPECT_FALSE(b.Get(i));
        EXPECT_FALSE(b[i]);
    }

    // Flip
    for (int i = 0; i < b.Size(); ++i)
    {
        b.Flip(i);
        EXPECT_TRUE(b.Get(i));
    }

    // SetAll
    b.SetAll(true);
    for (int i = 0; i < b.Size(); ++i)
    {
        bool result = b.Get(i);
        EXPECT_TRUE(result);
    }
    b.SetAll(false);
    for (int i = 0; i < b.Size(); ++i)
    {
        EXPECT_FALSE(b.Get(i));
    }
    
    // GetFirstUnsetIndex
    b.SetAll(false);
    EXPECT_EQ(b.GetFirstUnsetIndex(), 0);
    b.SetAll(true);
    EXPECT_EQ(b.GetFirstUnsetIndex(), -1);
    b.Unset(5);
    EXPECT_EQ(b.GetFirstUnsetIndex(), 5);
    
    // GetFirstSetIndex
    b.SetAll(true);
    EXPECT_EQ(b.GetFirstSetIndex(), 0);
    b.SetAll(false);
    EXPECT_EQ(b.GetFirstSetIndex(), -1);
    b.Set(5);
    EXPECT_EQ(b.GetFirstSetIndex(), 5);
    
    // GetNextUnsetIndex
    b.SetAll(false);
    for (int i = 0; i < b.Size(); ++i)
    {
        EXPECT_EQ(b.GetNextUnsetIndex(i), i);
    }
    
    // SetNextUnsetIndex
    b.SetAll(false);
    b.SetNextUnsetIndex(0);
    EXPECT_TRUE(b.Get(0));
    b.SetNextUnsetIndex(5);
    EXPECT_TRUE(b.Get(5));
    b.SetAll(true);
    b.Unset(3);
    b.SetNextUnsetIndex(5);
    EXPECT_FALSE(b.Get(3));
    b.SetNextUnsetIndex(1);
    EXPECT_TRUE(b.Get(3));
    b.SetAll(false);
    for (int i = 0; i < b.Size(); ++i)
    {
        b.SetNextUnsetIndex(i);
        EXPECT_TRUE(b[i]);
    }
    
    // GetNextSetIndex
    b.SetAll(true);
    EXPECT_EQ(b.GetNextSetIndex(0), 1);
    EXPECT_EQ(b.GetNextSetIndex(5), 6);
    EXPECT_EQ(b.GetNextSetIndex(b.Size()), -1);
    
    // CountSetBits
    b.SetAll(true);
    EXPECT_EQ(b.CountSetBits(), b.Size());
    b.SetAll(false);
    EXPECT_EQ(b.CountSetBits(), 0);
    b.Set(5);
    EXPECT_EQ(b.CountSetBits(), 1);
}
