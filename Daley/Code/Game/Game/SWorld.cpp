// Bradley Christensen - 2022-2026
#include "SWorld.h"
#include "BiomeDef.h"
#include "MapGeneratorDef.h"
#include "MapGenerator.h"
#include "SCWorld.h"
#include "SCFlowField.h"
#include "TileDef.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Math/Noise.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Startup()
{
	TileDef::LoadFromXML();
	BiomeDef::LoadFromXML();
	MapGeneratorDef::LoadFromXML();

	DevConsoleUtils::AddDevConsoleCommand("GenerateMap", &SWorld::GenerateMap, "mapGenName", DevConsoleArgType::String, "seed", DevConsoleArgType::Int);

	NamedProperties props;
	props.Set<std::string>("mapGenName", "forestCross");
	props.Set<int>("seed", 0);
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
	scWorld.m_solidnessChanged = false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SWorld::GenerateMap(NamedProperties& params)
{
	std::string mapGenName = params.Get<std::string>("mapGenName", "");
	int seed = params.Get<int>("seed", 0);

	MapGeneratorDef const* def = MapGeneratorDef::GetMapGeneratorDef(mapGenName);
	if (def == nullptr)
	{
		return false;
	}

	SCWorld& world = g_ecs->GetSingleton<SCWorld>();

	MapGenerator mapGenerator;
	mapGenerator.Initialize(*def, seed);

	mapGenerator.GenerateMap(world);

	SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();
	scFlowField.m_toGoalFlowField.Reset();

	return false;
}
