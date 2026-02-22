// Bradley Christensen - 2022-2026
#include "AssetID.h"



//----------------------------------------------------------------------------------------------------------------------
AssetID AssetID::Invalid = AssetID(UINT32_MAX);



//----------------------------------------------------------------------------------------------------------------------
AssetID::AssetID(uint32_t v) noexcept : value(v) 
{

}



//----------------------------------------------------------------------------------------------------------------------
AssetID::operator uint32_t() const noexcept
{
	return value;
}



//----------------------------------------------------------------------------------------------------------------------
AssetID AssetID::operator++(int) noexcept
{
    AssetID temp = *this;   // copy old value
    ++value;                // increment self
    return temp;            // return old value
}