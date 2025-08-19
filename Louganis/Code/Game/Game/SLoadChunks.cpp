// Bradley Christensen - 2023
#include "SLoadChunks.h"
#include "SCWorld.h"
#include "CPlayerController.h"
#include "CTransform.h"
#include "EntityDef.h"
#include "Chunk.h"
#include "TileDef.h"
#include "Engine/Debug/DevConsole.h"
#include <unordered_set>



//----------------------------------------------------------------------------------------------------------------------
namespace std
{
	template<>
	struct hash<IntVec2>
	{
		size_t operator()(const IntVec2& v) const noexcept
		{
			return (std::hash<int>()(v.x) << 1) ^ std::hash<int>()(v.y);
		}
	};
}



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

	int numTilesInRow = world.GetNumTilesInRow();
	float chunkWidth = numTilesInRow * worldSettings.m_tileWidth;
	float chunkLoadRadiusSquared = worldSettings.m_chunkLoadRadius * worldSettings.m_chunkLoadRadius;
	float chunkUnloadRadiusSquared = worldSettings.m_chunkUnloadRadius * worldSettings.m_chunkUnloadRadius;
	if (chunkUnloadRadiusSquared < chunkLoadRadiusSquared)
	{
		g_devConsole->LogWarning("Chunk unload radius was smaller than the load radius, clamping to match");
		chunkUnloadRadiusSquared = chunkLoadRadiusSquared;
	}

	for (auto it = g_ecs->Iterate<CTransform, CPlayerController>(context); it.IsValid(); ++it)
	{
		CTransform& playerTransform = *transformStorage.Get(it);

		int loadedChunks = 0;
		std::queue<IntVec2> openList;
		std::unordered_set<IntVec2> closedList;

		openList.push(world.GetChunkCoordsAtLocation(playerTransform.m_pos));

		while (!openList.empty())
		{
			IntVec2 top = openList.front();
			openList.pop();
			closedList.insert(top);

			AABB2 chunkBounds = world.CalculateChunkBounds(top.x, top.y);
			if (chunkBounds.GetCenter().GetDistanceSquaredTo(playerTransform.m_pos) < chunkLoadRadiusSquared)
			{
				if (world.TryLoadChunk(top)) 
				{
					loadedChunks++;
					if (loadedChunks >= world.m_worldSettings.m_maxNumChunksToLoadPerFrame)
					{
						break;
					}
				}

				IntVec2 neighbors[4] = { IntVec2(1, 0), IntVec2(-1, 0), IntVec2(0, 1), IntVec2(0, -1) };
				for (IntVec2& neighbor : neighbors)
				{
					IntVec2 neighborWorld = top + neighbor;
					if (closedList.insert(neighborWorld).second)
					{
						openList.push(neighborWorld);
					}
				}
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SLoadChunks::Shutdown()
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	world.ClearActiveChunks();
}
