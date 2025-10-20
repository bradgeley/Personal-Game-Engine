// Bradley Christensen - 2022-2025
#include "SDebugCommands.h"
#include "CAIController.h"
#include "CCollision.h"
#include "CHealth.h"
#include "CPlayerController.h"
#include "CTime.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "SCDebug.h"
#include "SCLoadChunks.h"
#include "SCTime.h"
#include "TileDef.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Startup()
{
	DevConsoleUtils::AddDevConsoleCommand("DamagePlayer", &SDebugCommands::DamagePlayer, "amount", DevConsoleArgType::Float);
	DevConsoleUtils::AddDevConsoleCommand("GodMode", &SDebugCommands::GodMode);
	DevConsoleUtils::AddDevConsoleCommand("Goto", &SDebugCommands::Goto, "location", DevConsoleArgType::Vec2);
	DevConsoleUtils::AddDevConsoleCommand("Ghost", &SDebugCommands::Ghost);
	DevConsoleUtils::AddDevConsoleCommand("SlowPlayer", &SDebugCommands::SlowPlayer, "slow", DevConsoleArgType::Bool);
	DevConsoleUtils::AddDevConsoleCommand("SetSeed", &SDebugCommands::SetSeed, "seed", DevConsoleArgType::Int);	
	DevConsoleUtils::AddDevConsoleCommand("SetDebugTileDef", &SDebugCommands::SetDebugTileDef, "tileDefName", DevConsoleArgType::String);
	DevConsoleUtils::AddDevConsoleCommand("SetTimeOfDay", &SDebugCommands::SetTimeOfDay, "timeOfDay", DevConsoleArgType::Int);
	DevConsoleUtils::AddDevConsoleCommand("KillAllEnemies", &SDebugCommands::KillAllEnemies);
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Run(SystemContext const&)
{
    // Empty
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugCommands::Shutdown()
{
	DevConsoleUtils::RemoveDevConsoleCommand("DamagePlayer", &SDebugCommands::DamagePlayer);
	DevConsoleUtils::RemoveDevConsoleCommand("GodMode", &SDebugCommands::GodMode);
	DevConsoleUtils::RemoveDevConsoleCommand("Goto", &SDebugCommands::Goto);
	DevConsoleUtils::RemoveDevConsoleCommand("Goto", &SDebugCommands::Ghost);
	DevConsoleUtils::RemoveDevConsoleCommand("SlowPlayer", &SDebugCommands::SlowPlayer);
	DevConsoleUtils::RemoveDevConsoleCommand("SetSeed", &SDebugCommands::SetSeed);
	DevConsoleUtils::RemoveDevConsoleCommand("SetDebugTileDef", &SDebugCommands::SetDebugTileDef);
	DevConsoleUtils::RemoveDevConsoleCommand("SetTimeOfDay", &SDebugCommands::SetTimeOfDay);
	DevConsoleUtils::RemoveDevConsoleCommand("KillAllEnemies", &SDebugCommands::KillAllEnemies);
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::Goto(NamedProperties& args)
{
	Vec2 location = args.Get("location", Vec2::ZeroVector);
	for (auto it = g_ecs->IterateAll<CPlayerController, CTransform>(); it.IsValid(); ++it)
	{
		CTransform* transform = g_ecs->GetComponent<CTransform>(it);
		transform->m_pos = location;
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::Ghost(NamedProperties&)
{
	for (auto it = g_ecs->IterateAll<CPlayerController, CCollision>(); it.IsValid(); ++it)
	{
		CCollision* cCollision = g_ecs->GetComponent<CCollision>(it);
		cCollision->SetCollisionEnabled(!cCollision->IsCollisionEnabled());
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::GodMode(NamedProperties&)
{
	for (auto it = g_ecs->IterateAll<CPlayerController, CHealth>(); it.IsValid(); ++it)
	{
		CHealth* health = g_ecs->GetComponent<CHealth>(it);
		health->SetInvincible(!health->GetIsInvincible());
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
	for (auto it = g_ecs->IterateAll<CPlayerController, CTime>(); it.IsValid(); ++it)
	{
		CTime* cTime = g_ecs->GetComponent<CTime>(it);
		cTime->m_clock.SetTimeDilation(slow ? 0.1 : 1.0);
	}
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::SetDebugTileDef(NamedProperties& args)
{
	std::string tileDefString = args.Get("tileDefName", std::string(""));
	Name tileDefName = Name(tileDefString);
	SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
	scDebug.m_debugPlacementTileID = tileDefName.IsValid() ? TileDef::GetTileDefID(tileDefName) : 0;
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::SetTimeOfDay(NamedProperties& args)
{
	int timeOfDay = args.Get("timeOfDay", -1);
	if (timeOfDay >= 0 && timeOfDay < (int) TimeOfDay::Count)
	{
		SCTime& scTime = g_ecs->GetSingleton<SCTime>();
		SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
		if ((int) scTime.m_timeOfDay <= timeOfDay)
		{
			// Advance 1 day
			scTime.m_dayCount++;
		}
		scTime.m_isTransitioning = false;
		scTime.m_timeOfDay = (TimeOfDay) timeOfDay;
		scTime.m_dayTimer.Set(nullptr, scWorld.m_worldSettings.m_timeOfDayDurations[(int) scTime.m_timeOfDay]);
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::DamagePlayer(NamedProperties& args)
{
	float damageAmount = args.Get("amount", 10.f);
	for (auto it = g_ecs->IterateAll<CPlayerController, CHealth>(); it.IsValid(); ++it)
	{
		CHealth* health = g_ecs->GetComponent<CHealth>(it);
		if (!health->GetIsInvincible())
		{
			health->m_currentHealth -= damageAmount;
		}
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugCommands::KillAllEnemies(NamedProperties&)
{
	for (auto it = g_ecs->IterateAll<CAIController, CHealth>(); it.IsValid(); ++it)
	{
		CHealth* health = g_ecs->GetComponent<CHealth>(it);
		if (!health->GetIsInvincible())
		{
			health->TakeDamage(health->m_currentHealth);
		}
	}
	return false;
}
