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
	for (int i = 0; i < 2; ++i)
	{
		playerInfo.m_spawnPos = Vec2(1 + 0.1f * (float)i, 1 + 0.1f * (float) i);
		entityFactory.m_entitiesToSpawn.push_back(playerInfo);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Run(SystemContext const&)
{

}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Shutdown()
{

}