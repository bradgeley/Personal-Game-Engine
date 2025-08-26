// Bradley Christensen - 2022-2025
#include "SSystemDebug.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/DataStructures/NamedProperties.h"



//----------------------------------------------------------------------------------------------------------------------
SSystemDebug::SSystemDebug(Name name, Rgba8 const& debugTint) : System(name, debugTint)
{
}



//----------------------------------------------------------------------------------------------------------------------
void SSystemDebug::Startup()
{
	ASSERT_OR_DIE(g_devConsole, "Dev Console doesn't exist.")
	ASSERT_OR_DIE(g_eventSystem, "Event System doesn't exist.")

	g_devConsole->AddDevConsoleCommandInfo("ToggleSystem", "name", DevConsoleArgType::String);

	g_eventSystem->SubscribeMethod("ToggleSystem", this, &SSystemDebug::ToggleSystem);
	g_eventSystem->SubscribeMethod("PrintAllSystems", this, &SSystemDebug::PrintAllSystems);
}



//----------------------------------------------------------------------------------------------------------------------
void SSystemDebug::Shutdown()
{
	ASSERT_OR_DIE(g_devConsole, "Dev Console doesn't exist.")
	ASSERT_OR_DIE(g_eventSystem, "Event System doesn't exist.")

	g_devConsole->RemoveDevConsoleCommandInfo("ToggleSystem");
	g_eventSystem->UnsubscribeMethod("ToggleSystem", this, &SSystemDebug::ToggleSystem);
}



//----------------------------------------------------------------------------------------------------------------------
bool SSystemDebug::ToggleSystem(NamedProperties& args)
{
	static std::string invalidName = "Invalid System";
	std::string name = args.Get("name", invalidName);
	System* system = g_ecs->GetSystemByName(name);
	if (system)
	{
		system->ToggleIsActive();
	}
	else
	{
		g_devConsole->AddLine(StringUtils::StringF("System (%s) does not exist", name.c_str()));
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SSystemDebug::PrintAllSystems(NamedProperties&)
{
	std::vector<SystemSubgraph> const& allSystemSubgraphs = g_ecs->GetSystemSubgraphs();

	for (int subgraphID = 0; subgraphID < (int) allSystemSubgraphs.size(); ++subgraphID)
	{
		SystemSubgraph const& subgraph = allSystemSubgraphs[subgraphID];
		g_devConsole->AddLine(StringUtils::StringF("Subgraph %i", subgraphID), Rgba8::LightOceanBlue);
		for (System* system : subgraph.m_systems)
		{
			std::string activeString = system->IsActive() ? "active" : "inactive";
			std::string line = StringUtils::StringF("- %s: (%s)", system->GetName().ToCStr(), activeString.c_str());
			g_devConsole->AddLine(line);
		}
	}
	return false;
}
