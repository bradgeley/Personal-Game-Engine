// Bradley Christensen - 2022-2025
#include "SUnloadChunks.h"
#include "SCWorld.h"
#include "CPlayerController.h"
#include "CTransform.h"
#include "EntityDef.h"
#include "Chunk.h"
#include "TileDef.h"
#include "Engine/Debug/DevConsole.h"



//----------------------------------------------------------------------------------------------------------------------
void SUnloadChunks::Startup()
{
	AddWriteDependencies<SCWorld, SCLoadChunks>();
	AddReadDependencies<CTransform, CPlayerController>();
}



//----------------------------------------------------------------------------------------------------------------------
void SUnloadChunks::Run(SystemContext const& context)
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	SCLoadChunks& scLoadChunks = g_ecs->GetSingleton<SCLoadChunks>();
	scLoadChunks.m_numUnloadedChunksThisFrame = 0;

	if (!world.GetPlayerChangedWorldCoordsThisFrame())
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

		if (chunkCenter.GetDistanceSquaredTo(playerTransform.m_pos) > unloadRadiusSquared)
		{
			chunkIt = world.m_activeChunks.erase(chunkIt);
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
