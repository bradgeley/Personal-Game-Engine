// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/BinaryUtils.h"
#include <cstdint>
#include <string.h> // for memset



//----------------------------------------------------------------------------------------------------------------------
constexpr int BIT_ARRAY_NUM_BITS_PER_ROW			= sizeof(size_t) * 8;;
constexpr int BIT_ARRAY_BYTE_MASK_LOWER_BITS		= BIT_ARRAY_NUM_BITS_PER_ROW - 1; 

// Shifting the index by this many bits is the same as dividing by the number of bits in a row, which gives us the index -> row conversion.
// e.g. on 64 bit, index 100 >> 6 = 1, so we know that the bit is found in row index 1, or the second 64 bit row.
#if INTPTR_MAX == INT64_MAX
constexpr int BIT_ARRAY_INDEX_TO_ROW_SHIFT = 6;
#else
constexpr int BIT_ARRAY_INDEX_TO_ROW_SHIFT = 5;
#endif



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
class BitArray
{
public:

	BitArray(bool initialValue = false);
	~BitArray() = default;

	static int Size();
	bool Get(int index) const;
	void Set(int index);
	void Unset(int index);
	bool Flip(int index);
	
	void SetAll(bool isSet);

	int GetFirstUnsetIndex() const;
	int GetFirstSetIndex() const;
	int GetNextUnsetIndex(int queryIndex) const;
	int SetNextUnsetIndex(int queryIndex);
	int GetNextSetIndex(int queryIndex) const;
	int CountSetBits() const;

	bool operator[](int index) const;

private:

	static inline int GetRowNumber(int index) { return index >> BIT_ARRAY_INDEX_TO_ROW_SHIFT; }
	static inline size_t GetBitMask(int index) { return size_t(1) << (index & BIT_ARRAY_BYTE_MASK_LOWER_BITS); } // same as: 1 << (index % NUM_BITS_IN_SIZET), which gets the remainder after dividing by the num bits in a row, which is the local bit index in that row.

private:

	static constexpr int s_numRows = (t_size / BIT_ARRAY_NUM_BITS_PER_ROW) + (t_size % BIT_ARRAY_NUM_BITS_PER_ROW > 0 ? 1 : 0);
	static constexpr size_t s_trailingBitMask = (SIZE_MAX >> t_size % BIT_ARRAY_NUM_BITS_PER_ROW) << (t_size % BIT_ARRAY_NUM_BITS_PER_ROW);

	size_t m_data[s_numRows] = { 0 };
};



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
BitArray<t_size>::BitArray(bool initialValue /*= false */)
{
	SetAll(initialValue);
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned t_size>
int BitArray<t_size>::Size()
{
	return (int) t_size;
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
bool BitArray<t_size>::Get(int index) const
{
	int byteIndex = GetRowNumber(index);
	return m_data[byteIndex] & GetBitMask(index);
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
void BitArray<t_size>::Set(int index)
{
	int byteIndex = GetRowNumber(index);
	m_data[byteIndex] |= GetBitMask(index);
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
void BitArray<t_size>::Unset(int index)
{
	int byteIndex = GetRowNumber(index);
	m_data[byteIndex] &= ~GetBitMask(index);
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
bool BitArray<t_size>::Flip(int index)
{
	bool isBitSet = Get(index);
	if (isBitSet)
	{
		Unset(index);
		return false;
	}
	else
	{
		Set(index);
		return true;
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
void BitArray<t_size>::SetAll(bool isSet)
{
	uint8_t fillValue = isSet ? 0xFF : 0;

	memset(&m_data, fillValue, s_numRows * sizeof(size_t));

	// Handle the trailing bits
	if (isSet)
	{
		m_data[s_numRows - 1] &= (~s_trailingBitMask); // make sure trailing values are all 0's so hamming weights are accurate
	}
	else
	{
		m_data[s_numRows - 1] &= (s_trailingBitMask); // make sure trailing values are all 0's so hamming weights are accurate
	}
}



//----------------------------------------------------------------------------------------------------------------------
template<unsigned int t_size>
int BitArray<t_size>::GetFirstUnsetIndex() const
{
	for (int currentRowIndex = 0; currentRowIndex < s_numRows; ++currentRowIndex)
	{
		size_t& row = m_data[currentRowIndex];

		int firstUnsetBit = BinaryUtils::FirstUnsetBit(row);

		if (firstUnsetBit != -1)
		{
			return firstUnsetBit;
	}
	}
	return -1;
}



//----------------------------------------------------------------------------------------------------------------------
template<unsigned int t_size>
int BitArray<t_size>::GetFirstSetIndex() const
{
	for (int currentRowIndex = 0; currentRowIndex < s_numRows; ++currentRowIndex)
	{
		size_t& row = m_data[currentRowIndex];

		int firstSetBit = BinaryUtils::FirstSetBit(row);

		if (firstSetBit != -1)
		{
			return firstSetBit;
		}
	}
	return -1;
}



//----------------------------------------------------------------------------------------------------------------------
template<unsigned int t_size>
int BitArray<t_size>::GetNextUnsetIndex(int queryIndex) const
{
	int firstRowIndex = GetRowNumber(queryIndex);
	int firstRowStartingBitIndex = queryIndex - BIT_ARRAY_NUM_BITS_PER_ROW * firstRowIndex; // After the first row, we start from index 0 in each subsequent row.

	for (int currentRowIndex = firstRowIndex; currentRowIndex < (int) s_numRows; ++currentRowIndex)
	{
		size_t const& row = m_data[currentRowIndex];

		int firstUnsetBit = -1;
		if (currentRowIndex == firstRowIndex)
		{
			firstUnsetBit = BinaryUtils::FirstUnsetBit(row, firstRowStartingBitIndex);
		}
		else
		{
			firstUnsetBit = BinaryUtils::FirstUnsetBit(row);
		}


		if (firstUnsetBit != -1)
		{
			return firstUnsetBit;
		}
	}
	return -1;
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
int BitArray<t_size>::SetNextUnsetIndex(int queryIndex)
{
	int nextUnsetIndex = GetNextUnsetIndex(queryIndex);
	if (nextUnsetIndex != -1)
	{
		Set(nextUnsetIndex);
		return nextUnsetIndex;
	}
	return -1;
}



//----------------------------------------------------------------------------------------------------------------------
template<unsigned int t_size>
int BitArray<t_size>::GetNextSetIndex(int queryIndex) const
{
	int firstRowIndex = GetRowNumber(queryIndex);
	int firstRowStartingBitIndex = queryIndex - BIT_ARRAY_NUM_BITS_PER_ROW * firstRowIndex; // After the first row, we start from index 0 in each subsequent row.

	for (int currentRowIndex = GetRowNumber(queryIndex); currentRowIndex < s_numRows; ++currentRowIndex)
	{
		size_t const& row = m_data[currentRowIndex];

		int firstSetBit = -1;
		if (currentRowIndex == firstRowIndex)
		{
			firstSetBit = BinaryUtils::FirstSetBit(row, firstRowStartingBitIndex);
		}
		else
		{
			firstSetBit = BinaryUtils::FirstSetBit(row);
		}

		if (firstSetBit != -1)
		{
			return firstSetBit;
		}
	}
	return -1;
}



//----------------------------------------------------------------------------------------------------------------------
template<unsigned int t_size>
int BitArray<t_size>::CountSetBits() const
{
	int count = 0;
	for (int i = 0; i < s_numRows; ++i)
	{
		size_t row = m_data[i];
		#if defined(_WIN64)
			count += (int) __popcnt64(static_cast<unsigned __int64>(row));
		#elif defined(_WIN32)
			count += (int) __popcnt(static_cast<unsigned int>(row));
		#elif defined(__clang__) || defined(__GNUC__)
			count += __builtin_popcountll(row);
		#else
			#error "No known population count function for this platform/compiler."
		#endif
	}

	return count;
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned t_size>
bool BitArray<t_size>::operator[](int index) const
{
	return Get(index);
}
