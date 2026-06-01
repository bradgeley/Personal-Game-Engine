// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/EntityID.h"
#include <functional>



//----------------------------------------------------------------------------------------------------------------------
struct OverlapInfo
{
	OverlapInfo(EntityID entityA, EntityID entityB) : m_entityA(entityA), m_entityB(entityB) {}

	EntityID m_entityA;
	EntityID m_entityB;

	bool operator==(OverlapInfo const& other) const
	{
		return (m_entityA == other.m_entityA && m_entityB == other.m_entityB) ||
			(m_entityA == other.m_entityB && m_entityB == other.m_entityA);
	}
};



//----------------------------------------------------------------------------------------------------------------------
template<>
struct std::hash<OverlapInfo>
{
	size_t operator()(const OverlapInfo& info) const noexcept
	{
		size_t hashA = std::hash<EntityID>{}(info.m_entityA);
		size_t hashB = std::hash<EntityID>{}(info.m_entityB);
		return hashA ^ (hashB << 1);
	}
};