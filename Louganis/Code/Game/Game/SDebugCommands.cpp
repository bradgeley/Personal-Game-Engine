// Bradley Christensen - 2022-2025
#include "SDebugCommands.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "CPlayerController.h"
#include "CTime.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Startup()
{
    AddWriteAllDependencies();

	DevConsoleUtils::AddDevConsoleCommand("SlowPlayer", &SDebugCommands::SlowPlayer, "slow", DevConsoleArgType::Bool);
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Run(SystemContext const& context)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Shutdown()
{
	DevConsoleUtils::RemoveDevConsoleCommand("SlowPlayer", &SDebugCommands::SlowPlayer);
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
