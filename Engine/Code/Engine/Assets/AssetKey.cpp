// Bradley Christensen - 2022-2026
#include "AssetKey.h"



//----------------------------------------------------------------------------------------------------------------------
AssetKey::AssetKey(Name name, std::type_index type) : m_name(name), m_typeIndex(type)
{
}



//----------------------------------------------------------------------------------------------------------------------
bool AssetKey::operator==(const AssetKey& other) const
{
    return m_name == other.m_name && m_typeIndex == other.m_typeIndex;
}
