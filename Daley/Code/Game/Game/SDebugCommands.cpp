// Bradley Christensen - 2022-2026
#include "SDebugCommands.h"
#include "EntityDef.h"
#include "SCEntityFactory.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Startup()
{
	DevConsoleUtils::AddDevConsoleCommand("Spawn", &SDebugCommands::Spawn, "name", DevConsoleArgType::String, "count", DevConsoleArgType::Int);
	DevConsoleUtils::AddDevConsoleCommand("DumpEntityDebug", &SDebugCommands::DumpEntityDebug);
	DevConsoleUtils::AddDevConsoleCommand("SetDebugPlacementEntity", &SDebugCommands::SetDebugPlacementEntity, "name", DevConsoleArgType::String);
	DevConsoleUtils::AddDevConsoleCommand("SlowAllEnemies", &SDebugCommands::SlowAllEnemies, "duration", DevConsoleArgType::Float);

	m_ignoreRun = true;
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Shutdown() const
{
	DevConsoleUtils::RemoveDevConsoleCommand("Spawn", &SDebugCommands::Spawn);
	DevConsoleUtils::RemoveDevConsoleCommand("DumpEntityDebug", &SDebugCommands::DumpEntityDebug);
	DevConsoleUtils::RemoveDevConsoleCommand("SetDebugPlacementEntity", &SDebugCommands::SetDebugPlacementEntity);
	DevConsoleUtils::RemoveDevConsoleCommand("SlowAllEnemies", &SDebugCommands::SlowAllEnemies);
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
	spawnCount = MathUtils::Clamp(spawnCount, 0, (int) MAX_ENTITIES - g_ecs->Count() + 1); // + 1 to show one error message if spawning past MAX_ENTITIES
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
	auto& debugStorage = g_ecs->GetArrayStorage<CEntityDebug>();
	for (auto it = g_ecs->IterateAll<CEntityDebug>(); it.IsValid(); ++it)
	{
		CEntityDebug const& debugComp = debugStorage[it];
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



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::SlowAllEnemies(NamedProperties& args)
{
	auto& timeStorage = g_ecs->GetArrayStorage<CTime>();
	float duration = args.Get("duration", 5.f);

	for (auto it = g_ecs->IterateAll<CTime>(); it.IsValid(); ++it)
	{
		CTime& time = timeStorage[it];
		time.m_remainingSlowDuration += duration;
	}

	return false;
}
