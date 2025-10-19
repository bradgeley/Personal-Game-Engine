// Bradley Christensen - 2022-2025
#include "SWorld.h"
#include "Chunk.h"
#include "CPlayerController.h"
#include "CTransform.h"
#include "EntityDef.h"
#include "SCEntityFactory.h"
#include "SCWorld.h"
#include "Engine/Math/RandomNumberGenerator.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Startup()
{
	AddWriteDependencies<SCWorld>();
	AddReadDependencies<CTransform>();

	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	if (world.m_worldSettings.m_randomWorldSeed)
	{
		world.m_worldSettings.m_worldSeed = g_rng->GenerateRandomSeed();
	}

	SCEntityFactory& entityFactory = g_ecs->GetSingleton<SCEntityFactory>();

	SpawnInfo playerInfo;
	playerInfo.m_def = EntityDef::GetEntityDef("Player");
	playerInfo.m_spawnPos = Vec2(1 + 0.1f, 1 + 0.1f);
	entityFactory.m_entitiesToSpawn.push_back(playerInfo);
}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Run(SystemContext const& context)
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();

	auto it = g_ecs->Iterate<CTransform, CPlayerController>(context);
	if (it.IsValid())
	{
		Vec2 firstPlayerLocation = g_ecs->GetComponent<CTransform>(it)->m_pos;
		UpdateLastKnownPlayerLocation(world, firstPlayerLocation);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::EndFrame()
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	for (auto& it : world.m_activeChunks)
	{
		Chunk* chunk = it.second;
		chunk->m_solidnessChanged = false;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Shutdown()
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	world.ClearActiveChunks();
}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::UpdateLastKnownPlayerLocation(SCWorld& world, Vec2 const& playerLocation) const
{
	WorldCoords playerWorldCoords = world.GetWorldCoordsAtLocation(playerLocation);

	world.m_playerChangedWorldCoordsThisFrame = false;
	if (playerWorldCoords != world.m_lastKnownPlayerWorldCoords || !world.m_lastKnownPlayerWorldCoords.IsValid())
	{
		world.m_lastKnownPlayerWorldCoords = playerWorldCoords;
		world.m_playerChangedWorldCoordsThisFrame = true;
	}
}
