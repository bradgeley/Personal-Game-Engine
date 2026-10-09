// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/EntityID.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
// As this is a game focused on high entity counts, we need FAST collision, optimized for lots of enemies.
// 
// Hashing:
// This system is the fastest way to hash all entities (assigning them each 1 tile, with no array allocation needed)
// 
// Querying:
// To query for collisions in a radius, we first need to create a sorted list of all enemies by their bucket index.
// Then we make a list of indices pointing to where each bucket starts in the sorted list. 
// This allows us to quickly find all enemies in a given bucket, so our ForEachTileInRadius func can still run over all
// entities in a radius without the cache misses and allocations of a 1 array-per-tile approach.
//
class SCCollision
{
public:

	// Return true to keep iterating
	void ForEachEnemyInTile(int tileIndex, std::function<bool(EntityID)> const& callback) const;
	void ForEachBuildingInTile(int tileIndex, std::function<bool(EntityID)> const& callback) const;

public:

	// All enemies, sorted by their bucket index. Todo: have an array of these, 1 for each collision layer
	std::vector<EntityID> m_sortedEnemies; 

	// Index: Tile bucket index. E.g. index 0 == tile index 0 in the world
	// Value: Index into m_sortedEnemies for the first entity in that bucket. -1 if empty.
	// If the value at index 5 is 10, then the first entity in bucket 5 is m_sortedEnemies[10].
	std::vector<int> m_enemyBucketIndices;

	std::vector<int> m_activeEnemyBuckets; // List of buckets with live enemies in them at this moment
	std::vector<std::vector<EntityID>> m_enemySortBuckets; // index: tileIndex, value: list of enemies in that tile
};

