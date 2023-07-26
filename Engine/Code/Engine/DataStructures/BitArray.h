// Bradley Christensen - 2023
#pragma once
#include <cstdint>



constexpr int BIT_ARRAY_NUM_BITS_PER_ROW			= 64;
constexpr int BIT_ARRAY_NUM_BITS_TO_SHIFT			= 6;
constexpr int BIT_ARRAY_BYTE_MASK_LOWER_BITS		= BIT_ARRAY_NUM_BITS_PER_ROW - 1;



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
class BitArray
{
public:

	BitArray(bool initialValue = false);
	~BitArray() = default;

	static int Size();
	bool Get(int index) const;
	bool Set(int index);
	bool Unset(int index);
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

	static int GetHammingWeight(size_t row);
	static inline int GetNumRows() { return (t_size / 64) + (t_size % 64 > 0 ? 1 : 0); }
	static inline int GetRowNumber(int index) { return index >> BIT_ARRAY_NUM_BITS_TO_SHIFT; }
	static inline size_t GetByteMask(int index) { return 1i64 << (index & BIT_ARRAY_BYTE_MASK_LOWER_BITS); } // same as: 1 << (index % NUM_BITS_IN_SIZET)

private:

	uint64_t m_data[(t_size / 64) + (t_size % 64 > 0 ? 1 : 0)] = { 0 };
	uint64_t m_trailingBitMask = 0;
};



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
BitArray<t_size>::BitArray(bool initialValue /*= false */)
	: m_trailingBitMask(((size_t)-1i64 >> t_size % BIT_ARRAY_NUM_BITS_PER_ROW) << (t_size % BIT_ARRAY_NUM_BITS_PER_ROW))
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
	return m_data[byteIndex] & GetByteMask(index);
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
bool BitArray<t_size>::Set(int index)
{
	int byteIndex = GetRowNumber(index);
	return m_data[byteIndex] |= GetByteMask(index);
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
bool BitArray<t_size>::Unset(int index)
{
	int byteIndex = GetRowNumber(index);
	return m_data[byteIndex] &= ~GetByteMask(index);
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
bool BitArray<t_size>::Flip(int index)
{
	bool isBitSet = Get(index);
	if (isBitSet) return Unset(index);
	else return Set(index);
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
void BitArray<t_size>::SetAll(bool isSet)
{
	for (size_t& data : m_data)
	{
		if (isSet)
		{
			data = SIZE_MAX;
		}
		else data = 0;
	}

	// Handle the trailing bits that don't really matter
	if (m_trailingBitMask != SIZE_MAX)
	{
		if (isSet)
		{
			m_data[GetNumRows() - 1] &= (~m_trailingBitMask); // make sure trailing values are all 0's so hamming weights are accurate
		}
		else
		{
			m_data[GetNumRows() - 1] &= (m_trailingBitMask); // make sure trailing values are all 0's so hamming weights are accurate
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
template<unsigned int t_size>
int BitArray<t_size>::GetFirstUnsetIndex() const
{
	return GetNextUnsetIndex(0);
}



//----------------------------------------------------------------------------------------------------------------------
template<unsigned int t_size>
int BitArray<t_size>::GetFirstSetIndex() const
{
	return GetNextSetIndex(0);
}



//----------------------------------------------------------------------------------------------------------------------
int BitArray_GetRightMostUnsetBitIndex(uint64_t t);



//----------------------------------------------------------------------------------------------------------------------
template<unsigned int t_size>
int BitArray<t_size>::GetNextUnsetIndex(int queryIndex) const
{
	size_t numRows = GetNumRows();
	int firstRowIndex = GetRowNumber(queryIndex);
	int firstQueryOffset = queryIndex - BIT_ARRAY_NUM_BITS_PER_ROW * firstRowIndex;

	for (int currentRowIndex = firstRowIndex; currentRowIndex < (int) numRows; ++currentRowIndex)
	{
		size_t const& row = m_data[currentRowIndex];
		if (row != SIZE_MAX)
		{
			int nextUnsetRowOffset;
			if (currentRowIndex == firstRowIndex)
			{
				uint64_t tExcludingOffsetBits = row | (~(SIZE_MAX << firstQueryOffset));
				nextUnsetRowOffset = BitArray_GetRightMostUnsetBitIndex(tExcludingOffsetBits);
			}
			else
			{
				nextUnsetRowOffset = BitArray_GetRightMostUnsetBitIndex(row);
			}
			if (nextUnsetRowOffset == -1) continue;

			int nextUnset = BIT_ARRAY_NUM_BITS_PER_ROW * currentRowIndex + nextUnsetRowOffset;
			if (nextUnset >= t_size) return -1;
			return nextUnset;
		}
	}
	return -1;
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned int t_size>
int BitArray<t_size>::SetNextUnsetIndex(int queryIndex)
{
	int numRows = GetNumRows();
	int firstRowIndex = GetRowNumber(queryIndex);
	int firstQueryOffset = queryIndex - BIT_ARRAY_NUM_BITS_PER_ROW * firstRowIndex;

	for (int currentRowIndex = firstRowIndex; currentRowIndex < numRows; ++currentRowIndex)
	{
		size_t const& row = m_data[currentRowIndex];
		if (row != SIZE_MAX)
		{
			int nextUnsetRowOffset;
			if (currentRowIndex == firstRowIndex)
			{
				uint64_t tExcludingOffsetBits = row | (~(SIZE_MAX << firstQueryOffset));
				nextUnsetRowOffset = BitArray_GetRightMostUnsetBitIndex(tExcludingOffsetBits);
			}
			else
			{
				nextUnsetRowOffset = BitArray_GetRightMostUnsetBitIndex(row);
			}
			if (nextUnsetRowOffset == -1) continue;

			int nextUnset = BIT_ARRAY_NUM_BITS_PER_ROW * currentRowIndex + nextUnsetRowOffset;
			if (nextUnset >= t_size) return -1;

			Set(nextUnset);
			return nextUnset;
		}
	}
	return -1;
}



//----------------------------------------------------------------------------------------------------------------------
template<unsigned int t_size>
int BitArray<t_size>::GetNextSetIndex(int queryIndex) const
{
	//queryIndex++;
	int currentLocalBitIndex = queryIndex & BIT_ARRAY_BYTE_MASK_LOWER_BITS;
	int numRows = GetNumRows();

	for (int currentRowIndex = GetRowNumber(queryIndex); currentRowIndex < numRows; ++currentRowIndex)
	{
		size_t const& row = m_data[currentRowIndex];
		if (row == 0i64) continue;

		while (currentLocalBitIndex < BIT_ARRAY_NUM_BITS_PER_ROW)
		{
			if ((row >> currentLocalBitIndex) & 1i64)
			{
				return currentRowIndex * BIT_ARRAY_NUM_BITS_PER_ROW + currentLocalBitIndex;
			}
			currentLocalBitIndex++;
		}
		currentLocalBitIndex = 0;
	}
	return -1;
}



//----------------------------------------------------------------------------------------------------------------------
template<unsigned int t_size>
int BitArray<t_size>::CountSetBits() const
{
	int count = 0;
	for (int i = 0; i < GetNumRows(); ++i)
	{
		size_t row = m_data[i];
		count += GetHammingWeight(row);
	}

	return count;
}



//----------------------------------------------------------------------------------------------------------------------
template <unsigned t_size>
bool BitArray<t_size>::operator[](int index) const
{
	return Get(index);
}



//----------------------------------------------------------------------------------------------------------------------
template<unsigned int t_size>
int BitArray<t_size>::GetHammingWeight(size_t row)
{
	if (row == 0) return 0;
	if (row == SIZE_MAX) return BIT_ARRAY_NUM_BITS_PER_ROW;
	
	int count = 0;
	for (int j = 0; j < BIT_ARRAY_NUM_BITS_PER_ROW; ++j)
	{
		count += (int)(row & 1);
		row >>= 1;
	}
	return count;
}