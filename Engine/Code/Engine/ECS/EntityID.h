// Bradley Christensen - 2022-2025
#pragma once
#include <cstddef>
#include <cstdint>
#include "Config.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr int ENTITY_INDEX_BITS = 24;
constexpr int ENTITY_INDEX_LIMIT = (1 << ENTITY_INDEX_BITS) - 1;
constexpr int ENTITY_INDEX_MASK = ENTITY_INDEX_LIMIT;
static_assert(MAX_ENTITIES <= ENTITY_INDEX_LIMIT, "MAX_ENTITIES is too high to fit in EntityID index bits.");
static_assert(ENTITY_INDEX_BITS >= 16 && ENTITY_INDEX_BITS <= 31, "ENTITY_INDEX_BITS must be between 16 and 31.");



//----------------------------------------------------------------------------------------------------------------------
// EntityID
//
struct EntityID
{
public:

	EntityID() = default;
	explicit EntityID(uint32_t index) : m_generationAndId(index) {}
	explicit EntityID(uint32_t index, uint32_t generation) : m_generationAndId(generation << ENTITY_INDEX_BITS | (index & ENTITY_INDEX_MASK)) {};

	inline uint32_t GetIndex() const		{ return m_generationAndId & ENTITY_INDEX_MASK; }
	inline uint32_t GetGeneration() const	{ return m_generationAndId >> ENTITY_INDEX_BITS; }

	bool operator==(EntityID const& other) const { return m_generationAndId == other.m_generationAndId; }
	bool operator!=(EntityID const& other) const { return m_generationAndId != other.m_generationAndId; }

public:

	static EntityID Singleton;
	static EntityID Invalid;

private:

	// Generation is the top 8 bits, index is the bottom 24
	uint32_t m_generationAndId = MAX_ENTITIES;
};
