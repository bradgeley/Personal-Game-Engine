// Bradley Christensen - 2023
#include "SLoadChunks.h"
#include "SCWorld.h"
#include "CPlayerController.h"
#include "CTransform.h"
#include "EntityDef.h"
#include "Chunk.h"
#include "TileDef.h"
#include "Engine/Debug/DevConsole.h"



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
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	WorldSettings const& worldSettings = world.m_worldSettings;

	int numTilesInRow = world.GetNumTilesInRow();
	float chunkWidth = numTilesInRow * worldSettings.m_tileWidth;
	float oneOverChunkWidth = 1.f / chunkWidth;
	float chunkLoadRadiusSquared = worldSettings.m_chunkLoadRadius * worldSettings.m_chunkLoadRadius;
	float chunkUnloadRadiusSquared = worldSettings.m_chunkUnloadRadius * worldSettings.m_chunkUnloadRadius;
	if (chunkUnloadRadiusSquared < chunkLoadRadiusSquared)
	{
		g_devConsole->LogWarning("Chunk unload radius was smaller than the load radius, clamping to match");
		chunkUnloadRadiusSquared = chunkLoadRadiusSquared;
	}

	auto transformStorage = g_ecs->GetArrayStorage<CTransform>();
	for (auto it = g_ecs->Iterate<CTransform, CPlayerController>(context); it.IsValid(); ++it)
	{
		CTransform& playerTransform = *transformStorage.Get(it);
		
		AABB2 loadChunksBounds;
		loadChunksBounds.mins = playerTransform.m_pos - Vec2(world.m_worldSettings.m_chunkLoadRadius, world.m_worldSettings.m_chunkLoadRadius);
		loadChunksBounds.maxs = playerTransform.m_pos + Vec2(world.m_worldSettings.m_chunkLoadRadius, world.m_worldSettings.m_chunkLoadRadius);
		IntVec2 chunkCoordsAtMins = IntVec2((loadChunksBounds.mins * oneOverChunkWidth).GetFloor());
		IntVec2 chunkCoordsAtMaxs = IntVec2((loadChunksBounds.maxs * oneOverChunkWidth).GetFloor());

		for (int y = chunkCoordsAtMins.y; y <= chunkCoordsAtMaxs.y; ++y)
		{
			for (int x = chunkCoordsAtMins.x; x <= chunkCoordsAtMaxs.x; ++x)
			{
				AABB2 chunkBounds = world.CalculateChunkBounds(x, y);
				float distanceSquared = chunkBounds.GetCenter().GetDistanceSquaredTo(playerTransform.m_pos);
				if (distanceSquared <= chunkLoadRadiusSquared)
				{
					world.GetOrCreateActiveChunk(x, y);
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
