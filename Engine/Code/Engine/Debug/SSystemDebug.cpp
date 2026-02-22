// Bradley Christensen - 2022-2026
#include "SSystemDebug.h"
#include "Engine/Debug/DevConsoleUtils.h"
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
	ASSERT_OR_DIE(g_eventSystem, "Event System doesn't exist.")

	DevConsoleUtils::AddCommandInfo("ToggleSystem", "name", DevConsoleArgType::String);

	g_eventSystem->SubscribeMethod("ToggleSystem", this, &SSystemDebug::ToggleSystem);
	g_eventSystem->SubscribeMethod("PrintAllSystems", this, &SSystemDebug::PrintAllSystems);
}



//----------------------------------------------------------------------------------------------------------------------
void SSystemDebug::Shutdown()
{
	ASSERT_OR_DIE(g_eventSystem, "Event System doesn't exist.")

	DevConsoleUtils::RemoveCommandInfo("ToggleSystem");
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
		DevConsoleUtils::LogError("System (%s) does not exist", name.c_str());
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

		DevConsoleUtils::Log(Rgba8::LightOceanBlue, "Subgraph %i", subgraphID);

		for (System* system : subgraph.m_systems)
		{
			std::string activeString = system->IsActive() ? "active" : "inactive";
			DevConsoleUtils::Log(Rgba8::LightOceanBlue, "- %s: (%s)", system->GetName().ToCStr(), activeString.c_str());
		}
	}
	return false;
}
