// Bradley Christensen - 2022-2025
#include "SDebugCommands.h"
#include "CPlayerController.h"
#include "CTime.h"
#include "CTransform.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "SCWorld.h"
#include "SCLoadChunks.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Startup()
{
    AddWriteAllDependencies();

	DevConsoleUtils::AddDevConsoleCommand("SlowPlayer", &SDebugCommands::SlowPlayer, "slow", DevConsoleArgType::Bool);
	DevConsoleUtils::AddDevConsoleCommand("SetSeed", &SDebugCommands::SetSeed, "seed", DevConsoleArgType::Int);	
	DevConsoleUtils::AddDevConsoleCommand("Goto", &SDebugCommands::Goto, "location", DevConsoleArgType::Vec2);
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Run(SystemContext const&)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Shutdown()
{
	DevConsoleUtils::RemoveDevConsoleCommand("SlowPlayer", &SDebugCommands::SlowPlayer);
	DevConsoleUtils::RemoveDevConsoleCommand("SetSeed", &SDebugCommands::SetSeed);
	DevConsoleUtils::RemoveDevConsoleCommand("Goto", &SDebugCommands::Goto);
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::Goto(NamedProperties& args)
{
	Vec2 location = args.Get("location", Vec2::ZeroVector);
	SystemContext context;
	for (auto it = g_ecs->Iterate<CPlayerController, CTransform>(context); it.IsValid(); ++it)
	{
		CTransform* transform = g_ecs->GetComponent<CTransform>(it);
		transform->m_pos = location;
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::SetSeed(NamedProperties& args)
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	SCLoadChunks& scLoadChunks = g_ecs->GetSingleton<SCLoadChunks>();
	world.m_worldSettings.m_worldSeed = args.Get("seed", (int) world.m_worldSettings.m_worldSeed);
	world.m_isWorldSeedDirty = true;
	scLoadChunks.m_unloadedChunksInRadius = true;
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::SlowPlayer(NamedProperties& args)
{
	bool slow = args.Get("slow", true);
	SystemContext context;
	for (auto it = g_ecs->Iterate<CPlayerController, CTime>(context); it.IsValid(); ++it)
	{
		CTime* cTime = g_ecs->GetComponent<CTime>(it);
		cTime->m_clock.SetLocalTimeDilation(slow ? 0.1 : 1.0);
	}
    return false;
}
