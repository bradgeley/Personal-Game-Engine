// Bradley Christensen - 2022-2025
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
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Shutdown()
{
	DevConsoleUtils::RemoveDevConsoleCommand("Spawn", &SDebugCommands::Spawn);
	DevConsoleUtils::RemoveDevConsoleCommand("DumpEntityDebug", &SDebugCommands::DumpEntityDebug);
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

	int spawnCount = args.Get("count", 1);
	for (int i = 0; i < spawnCount; i++)
	{
		SpawnInfo spawnInfo;
		spawnInfo.m_def = def;
		spawnInfo.m_spawnPos = world.GetRandomSpawnLocation();
		SCEntityFactory& factory = g_ecs->GetSingleton<SCEntityFactory>();
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
		DevConsoleUtils::Log(Rgba8::Coral, "Entity %i: %s", entityID, debugComp.m_defName.ToCStr());
	}
	return false;
}
