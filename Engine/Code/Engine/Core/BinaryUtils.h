// Bradley Christensen - 2022-2025
#pragma once



//----------------------------------------------------------------------------------------------------------------------
// Binary Utils
//
// Helpful functions for working with raw bits and byts
//

namespace BinaryUtils
{
	int FirstSetBit(size_t mask);
	int FirstSetBit(size_t mask, int firstValidIndex);
	int FirstUnsetBit(size_t mask);
	int FirstUnsetBit(size_t mask, int firstValidIndex);
}