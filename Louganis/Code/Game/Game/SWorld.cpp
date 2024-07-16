// Bradley Christensen - 2023
#include "SWorld.h"
#include "SCWorld.h"
#include "SCEntityFactory.h"
#include "EntityDef.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Startup()
{
	AddWriteDependencies<SCWorld, SCEntityFactory>();

	TileDef::LoadFromXML();

	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	world.m_tileIds.Initialize(IntVec2(100, 100), 0);
	world.m_tileIds.SetEdges(1);
	world.m_offset = Vec2{ -50, -50 };
	world.m_tileSize = 1.f;

	SCEntityFactory& entityFactory = g_ecs->GetSingleton<SCEntityFactory>();

	SpawnInfo playerInfo;
	playerInfo.m_def = EntityDef::GetEntityDef("Player");
	entityFactory.m_entitiesToSpawn.push_back(playerInfo);
}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Run(SystemContext const&)
{

}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Shutdown()
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	world.m_tileIds.Clear();
}
