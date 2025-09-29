﻿// Bradley Christensen - 2022-2025
#include "SLoadChunks.h"
#include "SCLoadChunks.h"
#include "SCWorld.h"
#include "CPlayerController.h"
#include "CTransform.h"
#include "EntityDef.h"
#include "Chunk.h"
#include "SEntityFactory.h"
#include "SCEntityFactory.h"
#include "TileDef.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Performance/ScopedTimer.h"



//----------------------------------------------------------------------------------------------------------------------
void SLoadChunks::Startup()
{
	AddWriteAllDependencies();

	TileDef::LoadFromXML();
}



//----------------------------------------------------------------------------------------------------------------------
void SLoadChunks::Run(SystemContext const& context)
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	SCLoadChunks& scLoadChunks = g_ecs->GetSingleton<SCLoadChunks>();
	auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();

	scLoadChunks.m_numLoadedChunksThisFrame = 0;
	if (!world.m_isWorldSeedDirty && !(scLoadChunks.m_unloadedChunksInRadius || world.GetPlayerChangedWorldCoordsThisFrame()))
	{
		// This loop needs to run if:
		// A - chunk loading was completely finished
		// B - player moved coords, so maybe we need to load more chunks
		return;
	}

	if (world.m_activeChunks.empty())
	{
		world.m_isWorldSeedDirty = false;
	}

	WorldSettings const& worldSettings = world.m_worldSettings;
	float chunkLoadRadius = worldSettings.m_chunkLoadRadius;
	float chunkUnloadRadius = worldSettings.m_chunkUnloadRadius;
	if (chunkUnloadRadius < chunkLoadRadius)
	{
		g_devConsole->LogWarning("Chunk unload radius was smaller than the load radius, clamping to match");
		chunkUnloadRadius = chunkLoadRadius;
	}

	// Shared storage to avoid extra allocations
	std::vector<SpawnInfo> sharedEntitiesToSpawn;
	sharedEntitiesToSpawn.reserve(100);

	scLoadChunks.m_unloadedChunksInRadius = false;
	for (auto it = g_ecs->Iterate<CTransform, CPlayerController>(context); it.IsValid(); ++it)
	{
		CTransform& playerTransform = *transformStorage.Get(it);

 		world.ForEachChunkCoordsOverlappingCircle_InRadialOrder(playerTransform.m_pos, chunkLoadRadius, [&world, &scLoadChunks, &sharedEntitiesToSpawn](IntVec2 const& chunkCoords)
		{ 
			bool shouldContinueLooping = true;
			if (!world.IsChunkLoaded(chunkCoords) && !world.m_isWorldSeedDirty)
			{
				Chunk* chunk = world.LoadChunk(chunkCoords, sharedEntitiesToSpawn);
				scLoadChunks.m_numLoadedChunksThisFrame++;
				bool hitLimit = scLoadChunks.m_numLoadedChunksThisFrame >= StaticWorldSettings::s_maxNumChunksToLoadPerFrame;
				scLoadChunks.m_unloadedChunksInRadius = hitLimit;
				shouldContinueLooping = !hitLimit;

				for (SpawnInfo const& spawnInfo : sharedEntitiesToSpawn)
				{
					EntityID spawnedEntity = SEntityFactory::SpawnEntity(spawnInfo);
					if (spawnedEntity != ENTITY_ID_INVALID)
					{
						chunk->m_spawnedEntities.push_back(spawnedEntity);
					}
				}
				sharedEntitiesToSpawn.clear();
			}
			return shouldContinueLooping;
		});
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SLoadChunks::Shutdown()
{

}
