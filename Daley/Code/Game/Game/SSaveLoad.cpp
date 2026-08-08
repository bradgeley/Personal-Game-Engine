// Bradley Christensen - 2022-2026
#include "SSaveLoad.h"
#include "SCWorld.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Core/FileUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Events/EventSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SSaveLoad::Startup()
{
	AddWriteAllDependencies();

	DevConsoleUtils::AddDevConsoleCommand("save", SSaveLoad::SaveGame, "filepath", DevConsoleArgType::String);
	DevConsoleUtils::AddDevConsoleCommand("load", SSaveLoad::LoadGame, "filepath", DevConsoleArgType::String);
}



//----------------------------------------------------------------------------------------------------------------------
void SSaveLoad::Shutdown() const
{
	DevConsoleUtils::RemoveDevConsoleCommand("save", SSaveLoad::SaveGame);
	DevConsoleUtils::RemoveDevConsoleCommand("load", SSaveLoad::LoadGame);
}



//----------------------------------------------------------------------------------------------------------------------
void SSaveLoad::Run(SystemContext const&) const
{

}



//----------------------------------------------------------------------------------------------------------------------
bool SSaveLoad::SaveGame(NamedProperties& args)
{
	std::string filepath = args.Get<std::string>("filepath", std::string("Save/save.txt"));

	std::string saveString;
	SCWorld const& world = g_ecs->GetSingleton<SCWorld>();
	saveString += StringUtils::StringF("%i\n", world.m_seed);
	saveString += StringUtils::StringF("%s\n", world.m_mapDefName.ToCStr());

	// Todo:
	// 1. Save background tiles using RLE
	// 2. Save entities

	FileUtils::FileWriteFromString(filepath, saveString);
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SSaveLoad::LoadGame(NamedProperties& args)
{
	std::string filepath = args.Get<std::string>("filepath", std::string("Save/save.txt"));

	std::string saveString;
	FileUtils::FileReadToString(filepath, saveString);
	Strings lines = StringUtils::SplitStringOnDelimiter(saveString, '\n');

	int seed = StringUtils::StringToInt(lines[0]);
	Name mapDefName = Name(lines[1]);

	NamedProperties mapGenParams;
	mapGenParams.Set<std::string>("mapGenName", mapDefName.ToString());
	mapGenParams.Set<int>("seed", seed);
	g_eventSystem->FireEvent("GenerateMap", mapGenParams);

	return false;
}
