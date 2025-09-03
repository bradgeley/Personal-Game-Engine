// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/Name.h"
#include <cstddef>
#include <typeindex>



//----------------------------------------------------------------------------------------------------------------------
struct AssetKey
{
	AssetKey() = default;
	AssetKey(Name name, std::type_index type);

    Name m_name; // Name or path of the asset
    std::type_index m_typeIndex;

    bool operator==(const AssetKey& other) const
    {
        return m_name == other.m_name && m_typeIndex == other.m_typeIndex;
    }
};



//----------------------------------------------------------------------------------------------------------------------
struct AssetKeyHash
{
    size_t operator()(AssetKey const& k) const noexcept
    {
        size_t h1 = std::hash<std::type_index>{}(k.m_typeIndex);
        size_t h2 = std::hash<Name>{}(k.m_name);
        return h1 ^ (h2 << 1); // combine hashes
    }
};