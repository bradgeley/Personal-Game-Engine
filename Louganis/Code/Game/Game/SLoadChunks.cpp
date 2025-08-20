// Bradley Christensen - 2023
#include "SLoadChunks.h"
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
	AddWriteDependencies<SCWorld>();
	AddReadDependencies<CTransform, CPlayerController>();

	TileDef::LoadFromXML();
}



//----------------------------------------------------------------------------------------------------------------------
void SLoadChunks::Run(SystemContext const& context)
{
	auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	WorldSettings const& worldSettings = world.m_worldSettings;

	float chunkLoadRadius = worldSettings.m_chunkLoadRadius;
	float chunkUnloadRadius = worldSettings.m_chunkUnloadRadius;
	if (chunkUnloadRadius < chunkLoadRadius)
	{
		g_devConsole->LogWarning("Chunk unload radius was smaller than the load radius, clamping to match");
		chunkUnloadRadius = chunkLoadRadius;
	}

	for (auto it = g_ecs->Iterate<CTransform, CPlayerController>(context); it.IsValid(); ++it)
	{
		CTransform& playerTransform = *transformStorage.Get(it);

		int numChunksLoaded = 0;
 		world.ForEachChunkCoordsInCircle(playerTransform.m_pos, chunkLoadRadius, [&world, &numChunksLoaded](IntVec2 const& chunkCoords) 
		{ 
			if (!world.IsChunkLoaded(chunkCoords))
			{
				world.LoadChunk(chunkCoords);
				numChunksLoaded++;
				return numChunksLoaded <= world.m_worldSettings.m_maxNumChunksToLoadPerFrame;
			}
			return true;
		});
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SLoadChunks::Shutdown()
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	world.ClearActiveChunks();
}
