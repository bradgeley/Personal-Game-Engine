// Bradley Christensen - 2022-2026
#include "SInput.h"
#include "CPlaceable.h"
#include "EntityDef.h"
#include "GameState.h"
#include "SCCamera.h"
#include "SCEntityFactory.h"
#include "SCEventSystem.h"
#include "SCGameState.h"
#include "SCInputSystem.h"
#include "SCRunData.h"
#include "SCWaves.h"
#include "SCWindow.h"
#include "SCWorld.h"
#include "WorldSettings.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SInput::Startup()
{
	AddWriteAllDependencies();
}



//----------------------------------------------------------------------------------------------------------------------
void SInput::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCCamera const& camera = context.GetSingletonConst<SCCamera>();
	SCWindow const& scWindow = context.GetSingletonConst<SCWindow>();
	SCWorld const& world = context.GetSingletonConst<SCWorld>();
	SCWaves const& waves = context.GetSingletonConst<SCWaves>();
	SCRunData const& runData = context.GetSingletonConst<SCRunData>();

	// Write Dependencies
	SCEventSystem& scEventSystem = context.GetSingleton<SCEventSystem>();
	SCInputSystem& scInput = context.GetSingleton<SCInputSystem>();
	SCEntityFactory& factory = context.GetSingleton<SCEntityFactory>();
	SCGameState& game = context.GetSingleton<SCGameState>();

	Window const& window = *scWindow.GetWindow();
	EventSystem& eventSystem = *scEventSystem.GetEventSystem();
	InputSystem const& inputSystem = *scInput.GetInputSystem();
	GameState& gameState = *game.m_gameState;

	if (!window.HasFocus())
	{
		return;
	}

	// Update mouse position
	Vec2 relMousePos = inputSystem.GetMouseViewportRelativePosition(StaticWorldSettings::s_visibleWorldAspect);
	scInput.m_mouseViewportRelativePos = relMousePos;
	scInput.m_mouseWorldLocation = camera.m_worldCamera.ScreenToWorldOrtho(relMousePos);
	scInput.m_mouseTileCoords = world.GetTileCoordsAtWorldPosClamped(scInput.m_mouseWorldLocation);
	scInput.m_mouseIntersectionCoords = world.GetTileIntersectionCoordsAtWorldPos(scInput.m_mouseWorldLocation);

	// Game Over?
	bool isVictory = waves.m_wavesFinished;
	bool isDefeat = runData.m_currentHealth <= 0.f;
	bool gameOver = isVictory || isDefeat;
	if (gameOver)
	{
		if (inputSystem.WasKeyJustPressed(KeyCode::Space))
		{
			NamedProperties changeStateProps;
			changeStateProps.Set<SCRunData>("RunData", runData);
			eventSystem.FireEvent("MissionOver", changeStateProps);
		}
		return;
	}

	// Tower Placement
	if (inputSystem.WasKeyJustPressed('T') && !scInput.m_isInTowerPlacementMode)
	{
		scInput.m_isInTowerPlacementMode = true;
	}

	if (scInput.m_isInTowerPlacementMode)
	{
		// Todo: maybe move even the creation of the tower placement info into STowerSpawner

		scInput.m_towerPlacementInfo = SInput::MakeTowerPlacementInfo(scInput.m_towerPlacementInfo.m_towerName, scInput.m_mouseWorldLocation, world);

		if (inputSystem.WasMouseButtonJustPressed(0))
		{
			factory.m_towerPlacements.push_back(scInput.m_towerPlacementInfo);
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
			if (!gameState.IsPaused())
			{
				gameState.TogglePaused();
			}
			else
			{
				gameState.RequestStateChange("MainMenu");
			}
		}
	}

	// Pause
	if (inputSystem.WasKeyJustPressed(KeyCode::Space))
	{
		gameState.TogglePaused();
	}
}



//----------------------------------------------------------------------------------------------------------------------
TowerPlacementInfo SInput::MakeTowerPlacementInfo(Name towerDefName, Vec2 const& worldPos, SCWorld const& world)
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