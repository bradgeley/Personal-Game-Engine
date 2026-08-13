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
		scInput.m_towerPlacementIndex = -1;

		if (inputSystem.WasKeyJustPressed(KeyCode::Space))
		{
			NamedProperties changeStateProps;
			changeStateProps.Set<SCRunData>("RunData", runData);
			eventSystem.FireEvent("MissionOver", changeStateProps);
		}
		return;
	}

	// Tower Placement
	for (int towerIndex = 0; towerIndex < static_cast<int>(runData.m_placeableTowers.size()); ++towerIndex)
	{
		PlaceableTower const& towerInfo = runData.m_placeableTowers[towerIndex];
		if (towerInfo.m_towerName == Name::Invalid)
		{
			continue;
		}

		if (inputSystem.WasKeyJustPressed(towerInfo.m_hotkey))
		{
			if (scInput.m_towerPlacementIndex != towerIndex)
			{
				scInput.m_towerPlacementIndex = towerIndex;
			}
			else
			{
				scInput.m_towerPlacementIndex = -1;
			}
			break;
		}
	}

	if (scInput.m_towerPlacementIndex != -1)
	{
		PlaceableTower const& placeableTower = runData.m_placeableTowers[scInput.m_towerPlacementIndex];
		bool canAfford = SInput::CanAffordTower(placeableTower, runData);
		scInput.m_towerPlacementRequest = SInput::MakeTowerPlacementRequest(placeableTower.m_towerName, scInput.m_mouseWorldLocation, world, false, placeableTower.m_cost, canAfford);
	}
	else
	{
		scInput.m_towerPlacementRequest = TowerPlacementRequest();
	}

	if (scInput.m_towerPlacementIndex != -1)
	{
		if (inputSystem.WasMouseButtonJustPressed(0))
		{
			factory.m_towerPlacements.push_back(scInput.m_towerPlacementRequest);
			scInput.m_towerPlacementIndex = -1; 
		}
		else if (inputSystem.WasMouseButtonJustPressed(1))
		{
			scInput.m_towerPlacementIndex = -1;
		}
		else if (inputSystem.WasKeyJustPressed(KeyCode::Escape))
		{
			scInput.m_towerPlacementIndex = -1;
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
bool SInput::CanAffordTower(PlaceableTower const& tower, SCRunData const& runData)
{
	return runData.m_gold + runData.m_creditLimit >= tower.m_cost;
}



//----------------------------------------------------------------------------------------------------------------------
TowerPlacementRequest SInput::MakeTowerPlacementRequest(Name towerDefName, Vec2 const& worldPos, SCWorld const& world, bool isGenerated /*= false*/, float cost /*= 0.f*/, bool canAfford /*= true*/)
{
	TowerPlacementRequest info;
	info.m_towerName = towerDefName;
	info.m_isGenerated = isGenerated;
	info.m_cost = cost;
	info.m_canAfford = canAfford;

	EntityDef const* def = EntityDef::GetEntityDef(towerDefName);
	ASSERT_OR_DIE(def != nullptr, "MakeTowerPlacementRequest: Invalid tower name for placement");
	ASSERT_OR_DIE(def->m_placeable.has_value(), "MakeTowerPlacementRequest: Tower entity def does not have a CPlaceable component");

	CPlaceable const& placeable = def->m_placeable.value();
	ASSERT_OR_DIE(placeable.m_dims.x > 0 && placeable.m_dims.y > 0, "MakeTowerPlacementRequest: Tower entity def has invalid dimensions");

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