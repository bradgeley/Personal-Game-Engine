// Bradley Christensen - 2022-2026
#include "SDebugInput.h"
#include "SCDebug.h"
#include "SCEntityFactory.h"
#include "SCWorld.h"
#include "SCInputSystem.h"
#include "TileDef.h"
#include "EntityDef.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Input/InputSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Startup()
{
	AddReadDependencies<SCInputSystem>();
	AddWriteDependencies<SCWorld, SCDebug, SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCInputSystem const& scInput = context.GetSingletonConst<SCInputSystem>();
	InputSystem const& input = *scInput.GetInputSystem();

	// Write Dependencies
	SCWorld& world = context.GetSingleton<SCWorld>();
	SCDebug& scDebug = context.GetSingleton<SCDebug>();
	SCEntityFactory& entityFactory = context.GetSingleton<SCEntityFactory>();

	// Debug Tile Placement
	if (input.IsKeyDown(KeyCode::Ctrl) && input.IsKeyDown('B'))
	{
		Tile newTile = TileDef::GetDefaultTile(scDebug.m_debugPlacementTileID);
		world.SetTile(scInput.m_mouseTileCoords, newTile);
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
			info.m_spawnPos = scInput.m_mouseWorldLocation;
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
