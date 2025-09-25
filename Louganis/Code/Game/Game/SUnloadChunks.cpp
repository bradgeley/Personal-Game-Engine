﻿// Bradley Christensen - 2022-2025
#include "SUnloadChunks.h"
#include "Chunk.h"
#include "CPlayerController.h"
#include "CTransform.h"
#include "EntityDef.h"
#include "SCEntityFactory.h"
#include "SCWorld.h"
#include "TileDef.h"
#include "Engine/Debug/DevConsole.h"



//----------------------------------------------------------------------------------------------------------------------
void SUnloadChunks::Startup()
{
	AddWriteDependencies<SCWorld, SCLoadChunks, SCEntityFactory>();
	AddReadDependencies<CTransform, CPlayerController>();
}



//----------------------------------------------------------------------------------------------------------------------
void SUnloadChunks::Run(SystemContext const& context)
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	SCLoadChunks& scLoadChunks = g_ecs->GetSingleton<SCLoadChunks>();
	SCEntityFactory& entityFactory = g_ecs->GetSingleton<SCEntityFactory>();
	scLoadChunks.m_numUnloadedChunksThisFrame = 0;

	if (!world.GetPlayerChangedWorldCoordsThisFrame() && !world.m_isWorldSeedDirty)
	{
		return;
	}

	auto playerIt = g_ecs->Iterate<CTransform, CPlayerController>(context);
	if (!playerIt.IsValid())
	{
		return;
	}

	auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();
	CTransform& playerTransform = *transformStorage.Get(playerIt);

	float unloadRadius = world.GetChunkUnloadRadius();
	float unloadRadiusSquared = unloadRadius * unloadRadius;

	int numChunksUnloaded = 0;
	for (auto chunkIt = world.m_activeChunks.begin(); chunkIt != world.m_activeChunks.end();)
	{
		Chunk* chunk = chunkIt->second;
		IntVec2 const& chunkCoords = chunkIt->first;
		Vec2 chunkCenter = world.CalculateChunkCenter(chunkCoords.x, chunkCoords.y);

		if (chunkCenter.GetDistanceSquaredTo(playerTransform.m_pos) > unloadRadiusSquared || world.m_isWorldSeedDirty)
		{
			chunkIt = world.m_activeChunks.erase(chunkIt);
			entityFactory.m_entitiesToDestroy.insert(entityFactory.m_entitiesToDestroy.end(), chunk->m_spawnedEntities.begin(), chunk->m_spawnedEntities.end());
			chunk->Destroy();
			delete chunk;
			numChunksUnloaded++;
		}
		else
		{
			chunkIt++;
		}
	}

	scLoadChunks.m_numUnloadedChunksThisFrame = numChunksUnloaded;
}



//----------------------------------------------------------------------------------------------------------------------
void SUnloadChunks::Shutdown()
{

}
