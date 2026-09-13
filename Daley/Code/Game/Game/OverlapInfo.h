// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/EntityID.h"
#include <functional>



//----------------------------------------------------------------------------------------------------------------------
struct OverlapInfo
{
	OverlapInfo(EntityID entityA, EntityID entityB);

	EntityID GetEntityA() const { return EntityID(static_cast<uint32_t>(m_key >> 32)); }
	EntityID GetEntityB() const { return EntityID(static_cast<uint32_t>(m_key & 0xFFFFFFFF)); }

	uint64_t m_key = 0;

	bool operator==(OverlapInfo const& other) const
	{
		return m_key == other.m_key;
	}
};



//----------------------------------------------------------------------------------------------------------------------
template<>
struct std::hash<OverlapInfo>
{
	size_t operator()(const OverlapInfo& info) const noexcept
	{
		return static_cast<size_t>(info.m_key);
	}
};