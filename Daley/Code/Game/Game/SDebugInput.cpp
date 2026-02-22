// Bradley Christensen - 2022-2026
#include "SDebugInput.h"
#include "SCCamera.h"
#include "SCDebug.h"
#include "SCEntityFactory.h"
#include "SCWorld.h"
#include "TileDef.h"
#include "EntityDef.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Startup()
{
	AddWriteAllDependencies();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Run(SystemContext const&)
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
	SCCamera& camera = g_ecs->GetSingleton<SCCamera>();
	SCEntityFactory& entityFactory = g_ecs->GetSingleton<SCEntityFactory>();

	// Debug mouse position
	if (g_window->HasFocus())
	{
		Vec2 relMousePos = g_input->GetMouseViewportRelativePosition(StaticWorldSettings::s_visibleWorldAspect);
		scDebug.m_debugMouseViewportRelativePos = relMousePos;
		scDebug.m_debugMouseWorldLocation = camera.m_camera.ScreenToWorldOrtho(relMousePos);
		scDebug.m_debugMouseTileCoords = world.GetTileCoordsAtWorldPosClamped(scDebug.m_debugMouseWorldLocation);
	}

	// Debug Tile Placement
	if (g_input->IsKeyDown(KeyCode::Ctrl) && g_input->IsKeyDown('B'))
	{
		Tile newTile = TileDef::GetDefaultTile(scDebug.m_debugPlacementTileID);
		world.SetTile(scDebug.m_debugMouseTileCoords, newTile);
	}

	// Debug Tower Placement
	if (g_input->IsKeyDown(KeyCode::Ctrl) && g_input->WasKeyJustPressed('T'))
	{
		EntityDef const* towerDef = EntityDef::GetEntityDef(scDebug.m_debugPlacementEntityName);
		if (!towerDef)
		{
			DevConsoleUtils::LogError("Invalid EntityDef name for debug tower placement: %s", scDebug.m_debugPlacementEntityName.ToCStr());
		}
		else
		{
			SpawnInfo info;
			info.m_def = EntityDef::GetEntityDef(scDebug.m_debugPlacementEntityName);
			info.m_spawnPos = scDebug.m_debugMouseWorldLocation;
			entityFactory.m_entitiesToSpawn.push_back(info);
		}
	}

	// Debug Enemy Spawning
	if (g_input->IsKeyDown(KeyCode::Ctrl) && g_input->IsKeyDown('W'))
	{
		NamedProperties args;
		args.Set<std::string>("name", "Ant");
		args.Set("count", 100);
		g_eventSystem->FireEvent("Spawn", args);
	}

	// Debug Enemy Spawning
	if (g_input->IsKeyDown(KeyCode::Ctrl) && g_input->IsKeyDown('K'))
	{
		for (auto it = g_ecs->IterateAll<CHealth>(); it.IsValid(); ++it)
		{
			SCEntityFactory& factory = g_ecs->GetSingleton<SCEntityFactory>();
			factory.m_entitiesToDestroy.push_back(it.GetEntityID());
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Shutdown()
{

}
