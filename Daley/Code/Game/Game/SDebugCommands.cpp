// Bradley Christensen - 2022-2026
#include "SDebugCommands.h"
#include "EntityDef.h"
#include "SCEntityFactory.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Startup()
{
	DevConsoleUtils::AddDevConsoleCommand("Spawn", &SDebugCommands::Spawn, "name", DevConsoleArgType::String, "count", DevConsoleArgType::Int);
	DevConsoleUtils::AddDevConsoleCommand("DumpEntityDebug", &SDebugCommands::DumpEntityDebug);
	DevConsoleUtils::AddDevConsoleCommand("SetDebugPlacementEntity", &SDebugCommands::SetDebugPlacementEntity, "name", DevConsoleArgType::String);
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Shutdown()
{
	DevConsoleUtils::RemoveDevConsoleCommand("Spawn", &SDebugCommands::Spawn);
	DevConsoleUtils::RemoveDevConsoleCommand("DumpEntityDebug", &SDebugCommands::DumpEntityDebug);
	DevConsoleUtils::RemoveDevConsoleCommand("SetDebugPlacementEntity", &SDebugCommands::SetDebugPlacementEntity);
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Run(SystemContext const&)
{
    // Empty
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::Spawn(NamedProperties& args)
{
	std::string name = args.Get("name", "");
	EntityDef const* def = EntityDef::GetEntityDef(name);
	if (!def)
	{
		DevConsoleUtils::LogError("No EntityDef found with name \"%s\"", name.c_str());
		return true;
	}
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	SCEntityFactory& factory = g_ecs->GetSingleton<SCEntityFactory>();

	int spawnCount = args.Get("count", 1);
	for (int i = 0; i < spawnCount; i++)
	{
		SpawnInfo spawnInfo;
		spawnInfo.m_def = def;
		spawnInfo.m_spawnPos = world.GetRandomSpawnLocation();
		factory.m_entitiesToSpawn.push_back(spawnInfo);
	}
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::DumpEntityDebug(NamedProperties&)
{
	for (auto it = g_ecs->IterateAll<CEntityDebug>(); it.IsValid(); ++it)
	{
		CEntityDebug const& debugComp = *g_ecs->GetComponent<CEntityDebug>(it);
		EntityID entityID = it.GetEntityID();
		DevConsoleUtils::Log(Rgba8::Coral, "Entity %i, gen %i: %s", entityID.GetIndex(), entityID.GetGeneration(), debugComp.m_defName.ToCStr());
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::SetDebugPlacementEntity(NamedProperties& args)
{
	std::string name = args.Get("name", "");
	EntityDef const* def = EntityDef::GetEntityDef(name);
	if (def)
	{
		SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
		scDebug.m_debugPlacementEntityName = name;
	}
	return false;
}
