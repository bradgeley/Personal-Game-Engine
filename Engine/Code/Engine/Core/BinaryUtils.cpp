// Bradley Christensen - 2025
#include "BinaryUtils.h"
#include <intrin.h>



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