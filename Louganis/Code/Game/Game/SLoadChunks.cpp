// Bradley Christensen - 2023
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



//----------------------------------------------------------------------------------------------------------------------
void SLoadChunks::Startup()
{
	AddWriteDependencies<SCWorld, SCLoadChunks>();
	AddReadDependencies<CTransform, CPlayerController>();

	TileDef::LoadFromXML();
}



//----------------------------------------------------------------------------------------------------------------------
void SLoadChunks::Run(SystemContext const& context)
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	SCLoadChunks& scLoadChunks = g_ecs->GetSingleton<SCLoadChunks>();

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

 		world.ForEachChunkCoordsInCircle(playerTransform.m_pos, chunkLoadRadius, [&world, &scLoadChunks](IntVec2 const& chunkCoords)
		{ 
			bool shouldContinueLooping = true;
			if (!world.IsChunkLoaded(chunkCoords))
			{
				world.LoadChunk(chunkCoords);
				scLoadChunks.m_numLoadedChunksThisFrame++;
				bool hitLimit = scLoadChunks.m_numLoadedChunksThisFrame >= world.m_worldSettings.m_maxNumChunksToLoadPerFrame;
				scLoadChunks.m_unloadedChunksInRadius = hitLimit;
				shouldContinueLooping = !hitLimit;
			}
			return shouldContinueLooping;
		});
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SLoadChunks::Shutdown()
{

}
