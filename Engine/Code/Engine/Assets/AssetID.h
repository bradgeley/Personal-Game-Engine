// Bradley Christensen - 2022-2026
#pragma once
#include <cstdint>
#include <functional>



//----------------------------------------------------------------------------------------------------------------------
struct AssetID
{
public:

    AssetID(uint32_t v = 0) noexcept;

    operator uint32_t() const noexcept;

    // Postfix ++  ( x++ )
    AssetID operator++(int) noexcept;

public:

    static AssetID Invalid;

	uint32_t value = UINT32_MAX;
};



//----------------------------------------------------------------------------------------------------------------------
namespace std
{
    template<>
    struct hash<AssetID>
    {
        size_t operator()(const AssetID& x) const noexcept
        {
            return std::hash<uint32_t>{}(x.value);
        }
    };
}