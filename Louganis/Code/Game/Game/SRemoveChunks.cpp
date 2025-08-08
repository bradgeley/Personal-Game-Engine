// Bradley Christensen - 2023
#include "SRemoveChunks.h"
#include "SCWorld.h"
#include "CPlayerController.h"
#include "CTransform.h"
#include "EntityDef.h"
#include "Chunk.h"
#include "TileDef.h"
#include "Engine/Debug/DevConsole.h"



//----------------------------------------------------------------------------------------------------------------------
void SRemoveChunks::Startup()
{
	AddWriteDependencies<SCWorld>();
	AddReadDependencies<CTransform, CPlayerController>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRemoveChunks::Run(SystemContext const& context)
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();

	auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();
	float unloadRadius = world.GetChunkUnloadRadius();
	float unloadRadiusSquared = unloadRadius * unloadRadius;
	
	std::vector<IntVec2> chunksToRemove;
	chunksToRemove.reserve(world.m_activeChunks.size());

	for (auto& chunkIt : world.m_activeChunks)
	{
		IntVec2 const& chunkCoords = chunkIt.first;

		AABB2 chunkBounds = world.CalculateChunkBounds(chunkCoords.x, chunkCoords.y);

		bool isChunkInRangeOfAtLeastOnePlayer = false;
		for (auto it = g_ecs->Iterate<CTransform, CPlayerController>(context); it.IsValid(); ++it)
		{
			CTransform& playerTransform = *transformStorage.Get(it);
			float distanceSquared = chunkBounds.GetCenter().GetDistanceSquaredTo(playerTransform.m_pos);

			if (distanceSquared < unloadRadiusSquared)
			{
				if (world.GetActiveChunk(chunkCoords))
				{
					isChunkInRangeOfAtLeastOnePlayer = true;
					break;
				}
			}
		}

		if (!isChunkInRangeOfAtLeastOnePlayer)
		{
			chunksToRemove.push_back(chunkCoords);
		}
	}

	world.RemoveActiveChunks(chunksToRemove);
}



//----------------------------------------------------------------------------------------------------------------------
void SRemoveChunks::Shutdown()
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	world.ClearActiveChunks();
}
