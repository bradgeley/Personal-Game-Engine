// Bradley Christensen - 2023
#include "BitArray.h"



//----------------------------------------------------------------------------------------------------------------------
int BitArray_GetRightMostUnsetBitIndex(uint64_t t)
{
	// All bits are full - no need to iterate 64 times
	if (t == SIZE_MAX)
	{
		return -1;
	}

	for (int i = 0; i < BIT_ARRAY_NUM_BITS_PER_ROW; ++i)
	{
		if ((t & 0x0000'0000'0000'0001) != 1)
		{
			return i;
		}
		t >>= 1;
	}
	return -1;
}