// Bradley Christensen - 2022-2025
#include "SLoadChunks.h"
#include "SCLoadChunks.h"
#include "SCWorld.h"
#include "CPlayerController.h"
#include "CTransform.h"
#include "EntityDef.h"
#include "Chunk.h"
#include "TileDef.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Performance/ScopedTimer.h"

// Experimental
#include "SCEntityFactory.h"



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

	// Experimental
	SCEntityFactory& entityFactory = g_ecs->GetSingleton<SCEntityFactory>();

	scLoadChunks.m_numLoadedChunksThisFrame = 0;
	if (!(scLoadChunks.m_unloadedChunksInRadius || world.GetPlayerChangedWorldCoordsThisFrame()))
	{
		// This loop needs to run if:
		// A - chunk loading was completely finished
		// B - player moved coords, so maybe we need to load more chunks
		return;
	}

	auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();
	WorldSettings const& worldSettings = world.m_worldSettings;
	float chunkLoadRadius = worldSettings.m_chunkLoadRadius;
	float chunkUnloadRadius = worldSettings.m_chunkUnloadRadius;
	if (chunkUnloadRadius < chunkLoadRadius)
	{
		g_devConsole->LogWarning("Chunk unload radius was smaller than the load radius, clamping to match");
		chunkUnloadRadius = chunkLoadRadius;
	}

	scLoadChunks.m_unloadedChunksInRadius = false;
	for (auto it = g_ecs->Iterate<CTransform, CPlayerController>(context); it.IsValid(); ++it)
	{
		CTransform& playerTransform = *transformStorage.Get(it);

 		world.ForEachChunkCoordsOverlappingCircle_InRadialOrder(playerTransform.m_pos, chunkLoadRadius, [&world, &entityFactory, &scLoadChunks](IntVec2 const& chunkCoords)
		{ 
			bool shouldContinueLooping = true;
			if (!world.IsChunkLoaded(chunkCoords))
			{
				world.LoadChunk(chunkCoords);
				scLoadChunks.m_numLoadedChunksThisFrame++;
				bool hitLimit = scLoadChunks.m_numLoadedChunksThisFrame >= StaticWorldSettings::s_maxNumChunksToLoadPerFrame;
				scLoadChunks.m_unloadedChunksInRadius = hitLimit;
				shouldContinueLooping = !hitLimit;

				//f (Chunk* chunk = world.GetActiveChunk(chunkCoords))
				//
				//	for (int tileIndex = 0; tileIndex < StaticWorldSettings::s_numTilesInChunk; ++tileIndex)
				//	{
				//		if (!chunk->IsTileSolid(tileIndex))
				//		{
				//			// Experimental
				//			SpawnInfo spawnInfo;
				//			spawnInfo.m_def = EntityDef::GetEntityDef("BabyZombie");
				//			WorldCoords worldCoords(chunkCoords, chunk->m_tileIDs.GetCoordsForIndex(tileIndex));
				//			spawnInfo.m_spawnPos = world.GetTileBounds(worldCoords).GetCenter();
				//			entityFactory.m_entitiesToSpawn.emplace_back(spawnInfo);
				//		}
				//	}
				//
			}
			return shouldContinueLooping;
		});
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SLoadChunks::Shutdown()
{

}
