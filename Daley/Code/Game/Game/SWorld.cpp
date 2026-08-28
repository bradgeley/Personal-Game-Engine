// Bradley Christensen - 2022-2026
#include "SWorld.h"
#include "BiomeDef.h"
#include "MapGeneratorDef.h"
#include "MapGenerator.h"
#include "SCEntityFactory.h"
#include "SCFlowField.h"
#include "SCRunData.h"
#include "SCWorld.h"
#include "TileDef.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/ECS/AdminSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Startup()
{
	TileDef::LoadFromXML();
	BiomeDef::LoadFromXML();
	MapGeneratorDef::LoadFromXML();

	DevConsoleUtils::AddDevConsoleCommand("GenerateMap", &SWorld::GenerateMap, "mapGenName", DevConsoleArgType::Name, "seed", DevConsoleArgType::UInt);


	SCRunData const& scRunData = g_ecs->GetSingleton<SCRunData>();
	RunData const& runData = *scRunData.m_data;
	MissionGenData const& missionGenData = runData.m_missionGenData[runData.m_missionIndex];

	NamedProperties props;
	props.Set<Name>("mapGenName", missionGenData.m_mapName);
	props.Set<uint32_t>("seed", runData.m_seed + static_cast<uint32_t>(runData.m_missionIndex));
	GenerateMap(props);

	m_ignoreRun = true;
}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Shutdown() const
{
	MapGeneratorDef::Shutdown();
	BiomeDef::Shutdown();
	TileDef::Shutdown();

	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
	scWorld.Shutdown();

	DevConsoleUtils::RemoveDevConsoleCommand("GenerateMap", &SWorld::GenerateMap);

}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::EndFrame() const
{
	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
	scWorld.m_solidnessOfPathTileChanged = false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SWorld::GenerateMap(NamedProperties& params)
{
	Name mapGenName = params.Get<Name>("mapGenName", Name(""));
	uint32_t seed = params.Get<uint32_t>("seed", 0);

	MapGeneratorDef const* def = MapGeneratorDef::GetMapGeneratorDef(mapGenName);
	if (def == nullptr)
	{
		return false;
	}

	SCWorld& world = g_ecs->GetSingleton<SCWorld>();

	g_ecs->DestroyAllEntities();

	MapGenerator mapGenerator;
	mapGenerator.Initialize(*def, seed);

	mapGenerator.GenerateMap(world);

	SCEntityFactory& scEntityFactory = g_ecs->GetSingleton<SCEntityFactory>();
	scEntityFactory.m_towerPlacements.insert(scEntityFactory.m_towerPlacements.end(), world.m_generatedTowers.begin(), world.m_generatedTowers.end());

	SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();
	scFlowField.m_toGoalFlowField.Reset();

	return false;
}
