// Bradley Christensen - 2023
#include "SWorld.h"
#include "SCWorld.h"
#include "SCEntityFactory.h"
#include "EntityDef.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Startup()
{
	AddWriteDependencies<SCWorld, SCEntityFactory>();

	SCEntityFactory& entityFactory = g_ecs->GetSingleton<SCEntityFactory>();

	SpawnInfo playerInfo;
	playerInfo.m_def = EntityDef::GetEntityDef("Player");
	playerInfo.m_spawnPos = Vec2(1.1f, 1.1f);
	entityFactory.m_entitiesToSpawn.push_back(playerInfo);
}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Run(SystemContext const&)
{

}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Shutdown()
{

}