// Bradley Christensen - 2022-2025
#include "BinaryUtils.h"
#include <intrin.h>
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
int BinaryUtils::FirstSetBit(size_t mask)
{
	bool foundSetBit = false;
	unsigned long firstSetBitIndex = 0;

	#if defined(_M_X64) || defined(_WIN64)
		foundSetBit = _BitScanForward64(&firstSetBitIndex, static_cast<unsigned long long>(mask));
	#else
		foundSetBit = _BitScanForward(&firstSetBitIndex, static_cast<unsigned long>(mask));
	#endif

	if (!foundSetBit)
	{
		return -1;
	}

	return firstSetBitIndex;
}



//----------------------------------------------------------------------------------------------------------------------
int BinaryUtils::FirstSetBit(size_t mask, int firstValidIndex)
{
	if (mask == (size_t) 0)
	{
		return -1;
	}

	size_t& maskExcludingInvalidLowerBits = mask;

	size_t firstRowMask = SIZE_MAX << firstValidIndex >> firstValidIndex; // zero's out the least significant n (firstValidIndex) bits
	maskExcludingInvalidLowerBits &= firstRowMask; // &= to force zero out the LSB in the mask

	return FirstSetBit(maskExcludingInvalidLowerBits);
}



//----------------------------------------------------------------------------------------------------------------------
int BinaryUtils::FirstUnsetBit(size_t mask)
{
	bool foundUnsetBit = false;
	unsigned long firstUnsetBitIndex = 0;

	#if defined(_M_X64) || defined(_WIN64)
		foundUnsetBit = _BitScanForward64(&firstUnsetBitIndex, static_cast<unsigned long long>(~mask));
	#else
		foundUnsetBit = _BitScanForward(&firstUnsetBitIndex, static_cast<unsigned long>(~mask));
	#endif

	if (!foundUnsetBit)
	{
		return -1;
	}

	return firstUnsetBitIndex;
}



//----------------------------------------------------------------------------------------------------------------------
int BinaryUtils::FirstUnsetBit(size_t mask, int firstValidIndex)
{
	if (mask == SIZE_MAX)
	{
		return -1;
	}

	size_t& maskExcludingInvalidLowerBits = mask;

	size_t firstRowMask = SIZE_MAX >> firstValidIndex >> firstValidIndex; // zero's out the least significant n (firstValidIndex) bits
	firstRowMask = ~firstRowMask; // Turn all the LSB into 1's (0 would count as unset)
	maskExcludingInvalidLowerBits |= firstRowMask;

	return FirstUnsetBit(maskExcludingInvalidLowerBits);
}
