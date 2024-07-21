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
	WorldSettings const& worldSettings = world.m_worldSettings;

	auto transformStorage = g_ecs->GetArrayStorage<CTransform>();
	float unloadRadius = world.GetChunkUnloadRadius();
	float unloadRadiusSquared = unloadRadius * unloadRadius;
	
	std::vector<IntVec2> chunksToRemove;
	chunksToRemove.reserve(world.m_activeChunks.size());

	for (auto& it : world.m_activeChunks)
	{
		IntVec2 const& dims = it.first;
		Chunk* chunk = it.second;

		AABB2 chunkBounds = world.CalculateChunkBounds(dims.x, dims.y);
		for (auto it = g_ecs->Iterate<CTransform, CPlayerController>(context); it.IsValid(); ++it)
		{
			CTransform& playerTransform = *transformStorage.Get(it);
			float distanceSquared = chunkBounds.GetCenter().GetDistanceSquaredTo(playerTransform.m_pos);

			if (distanceSquared > unloadRadiusSquared)
			{
				Chunk* chunk = world.GetActiveChunk(dims);
				if (chunk)
				{
					chunksToRemove.push_back(dims);
				}
			}
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
