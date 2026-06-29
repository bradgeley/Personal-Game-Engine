// Bradley Christensen - 2022-2026
#include "SInput.h"
#include "CPlaceable.h"
#include "EntityDef.h"
#include "Game.h"
#include "SCCamera.h"
#include "SCEntityFactory.h"
#include "SCInputSystem.h"
#include "SCGame.h"
#include "SCWindow.h"
#include "SCWorld.h"
#include "WorldSettings.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
TowerPlacementInfo MakeTowerPlacementInfo(Name towerDefName, Vec2 const& worldPos, SCWorld const& world);



//----------------------------------------------------------------------------------------------------------------------
void SInput::Startup()
{
	AddReadDependencies<SCCamera, SCWindow>();
    AddWriteDependencies<SCEntityFactory, SCGame, SCInputSystem, SCWorld>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInput::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCCamera const& camera = context.GetSingletonConst<SCCamera>();
	SCWindow const& scWindow = context.GetSingletonConst<SCWindow>();

	// Write Dependencies
	SCEntityFactory& entityFactory = context.GetSingleton<SCEntityFactory>();
	SCInputSystem& scInput = context.GetSingleton<SCInputSystem>();
	SCWorld& world = context.GetSingleton<SCWorld>();
	SCGame& game = context.GetSingleton<SCGame>();

	Window const& window = *scWindow.GetWindow();
	InputSystem const& inputSystem = *scInput.GetInputSystem();
	Game& gameInstance = *game.m_game;

	if (!window.HasFocus())
	{
		return;
	}

	// Update mouse position
	Vec2 relMousePos = inputSystem.GetMouseViewportRelativePosition(StaticWorldSettings::s_visibleWorldAspect);
	scInput.m_mouseViewportRelativePos = relMousePos;
	scInput.m_mouseWorldLocation = camera.m_camera.ScreenToWorldOrtho(relMousePos);
	scInput.m_mouseTileCoords = world.GetTileCoordsAtWorldPosClamped(scInput.m_mouseWorldLocation);
	scInput.m_mouseIntersectionCoords = world.GetTileIntersectionCoordsAtWorldPos(scInput.m_mouseWorldLocation);

	// Tower Placement
	if (inputSystem.WasKeyJustPressed('T') && !scInput.m_isInTowerPlacementMode)
	{
		scInput.m_isInTowerPlacementMode = true;
	}

	if (scInput.m_isInTowerPlacementMode)
	{
		scInput.m_towerPlacementInfo = MakeTowerPlacementInfo(scInput.m_towerPlacementInfo.m_towerName, scInput.m_mouseWorldLocation, world);

		if (inputSystem.WasMouseButtonJustPressed(0))
		{
			world.PlaceTower(scInput.m_towerPlacementInfo, entityFactory);
			scInput.m_isInTowerPlacementMode = false; 
		}
		else if (inputSystem.WasMouseButtonJustPressed(1))
		{
			scInput.m_isInTowerPlacementMode = false;
		}
		else if (inputSystem.WasKeyJustPressed(KeyCode::Escape))
		{
			scInput.m_isInTowerPlacementMode = false;
		}
	}
	else
	{
		if (inputSystem.WasKeyJustPressed(KeyCode::Escape))
		{
			if (!game.m_game->IsPaused())
			{
				game.m_game->TogglePaused();
			}
			else
			{
				game.m_game->Quit();
			}
		}
	}

	// Pause
	if (inputSystem.WasKeyJustPressed(KeyCode::Space))
	{
		gameInstance.TogglePaused();
	}
}



//----------------------------------------------------------------------------------------------------------------------
TowerPlacementInfo MakeTowerPlacementInfo(Name towerDefName, Vec2 const& worldPos, SCWorld const& world)
{
	TowerPlacementInfo info;
	info.m_towerName = towerDefName;

	EntityDef const* def = EntityDef::GetEntityDef(towerDefName);
	ASSERT_OR_DIE(def != nullptr, "MakeTowerPlacementInfo: Invalid tower name for placement");
	ASSERT_OR_DIE(def->m_placeable.has_value(), "MakeTowerPlacementInfo: Tower entity def does not have a CPlaceable component");

	CPlaceable const& placeable = def->m_placeable.value();
	ASSERT_OR_DIE(placeable.m_dims.x > 0 && placeable.m_dims.y > 0, "MakeTowerPlacementInfo: Tower entity def has invalid dimensions");

	Vec2 halfDims = Vec2(static_cast<float>(placeable.m_dims.x) * 0.5f, static_cast<float>(placeable.m_dims.y) * 0.5f);
	Vec2 botLeft = worldPos - halfDims;
	IntVec2 botLeftIntersection = world.GetTileIntersectionCoordsAtWorldPos(botLeft); // Bot left has to sit neatly at a tile intersection - easier to calculate than the center
	info.m_botLeftTileCoords = botLeftIntersection;
	info.m_topRightTileCoords = botLeftIntersection + placeable.m_dims - IntVec2::OneVector;
	info.m_dims = placeable.m_dims;
	info.m_tileTagQuery = placeable.m_tileTagQuery;
	info.m_worldPos = world.GetWorldPosAtTileIntersectionCoords(botLeftIntersection) + halfDims;

	return info;
}