// Bradley Christensen - 2022-2026
#include "SDebugInput.h"
#include "SCCamera.h"
#include "SCDebug.h"
#include "SCEntityFactory.h"
#include "SCWorld.h"
#include "SCInputSystem.h"
#include "SCWindow.h"
#include "TileDef.h"
#include "EntityDef.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Events/EventSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Startup()
{
	AddReadDependencies<SCCamera, SCInputSystem, SCWindow>();
	AddWriteDependencies<SCWorld, SCDebug, SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCCamera const& camera = context.GetSingletonConst<SCCamera>();
	InputSystem const& input = *context.GetSingletonConst<SCInputSystem>().m_inputSystem;
	Window const& window = *context.GetSingletonConst<SCWindow>().m_window;

	// Write Dependencies
	SCWorld& world = context.GetSingleton<SCWorld>();
	SCDebug& scDebug = context.GetSingleton<SCDebug>();
	SCEntityFactory& entityFactory = context.GetSingleton<SCEntityFactory>();

	// Debug mouse position
	if (window.HasFocus())
	{
		Vec2 relMousePos = input.GetMouseViewportRelativePosition(StaticWorldSettings::s_visibleWorldAspect);
		scDebug.m_debugMouseViewportRelativePos = relMousePos;
		scDebug.m_debugMouseWorldLocation = camera.m_camera.ScreenToWorldOrtho(relMousePos);
		scDebug.m_debugMouseTileCoords = world.GetTileCoordsAtWorldPosClamped(scDebug.m_debugMouseWorldLocation);
	}

	// Debug Tile Placement
	if (input.IsKeyDown(KeyCode::Ctrl) && input.IsKeyDown('B'))
	{
		Tile newTile = TileDef::GetDefaultTile(scDebug.m_debugPlacementTileID);
		world.SetTile(scDebug.m_debugMouseTileCoords, newTile);
	}

	// Debug Tower Placement
	if (input.IsKeyDown(KeyCode::Ctrl) && input.WasKeyJustPressed('T'))
	{
		EntityDef const* def = EntityDef::GetEntityDef(scDebug.m_debugPlacementEntityName);
		if (!def)
		{
			DevConsoleUtils::LogError("Invalid EntityDef name for debug placement: %s", scDebug.m_debugPlacementEntityName.ToCStr());
		}
		else
		{
			SpawnInfo info;
			info.m_def = def;
			info.m_spawnPos = scDebug.m_debugMouseWorldLocation;
			entityFactory.m_entitiesToSpawn.push_back(info);
		}
	}

	// Debug Enemy Spawning
	if (input.IsKeyDown(KeyCode::Ctrl) && input.IsKeyDown('K'))
	{
		for (auto it = context.Iterate<CHealth>(); it.IsValid(); ++it)
		{
			entityFactory.m_entitiesToDestroy.push_back(it.GetEntityID());
		}
	}
}
