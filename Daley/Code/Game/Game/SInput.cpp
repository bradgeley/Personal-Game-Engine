// Bradley Christensen - 2022-2026
#include "SInput.h"
#include "CPlaceable.h"
#include "EntityDef.h"
#include "GameCommon.h"
#include "GameState.h"
#include "SCCamera.h"
#include "SCEntityFactory.h"
#include "SCEventSystem.h"
#include "SCFloatingText.h"
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
#include "Engine/Time/Clock.h"
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
	auto& placeableStorage = context.GetMapStorageConst<CPlaceable>();
	auto& transformStorage = context.GetArrayStorageConst<CTransform>();

	// Write Dependencies
	SCEventSystem& scEventSystem = context.GetSingleton<SCEventSystem>();
	SCInputSystem& scInput = context.GetSingleton<SCInputSystem>();
	SCEntityFactory& factory = context.GetSingleton<SCEntityFactory>();
	SCRunData& scRunData = context.GetSingleton<SCRunData>();
	RunData& runData = *scRunData.m_data;
	SCFloatingText& floatingText = context.GetSingleton<SCFloatingText>();
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

	// HUD Toggle
	if (inputSystem.WasKeyJustPressed('H'))
	{
		SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
		scRenderer.m_isHudEnabled = !scRenderer.m_isHudEnabled;
	}

	// Game Over?
	bool isVictory = waves.m_wavesFinished;
	bool isDefeat = runData.m_health <= 0.f;
	bool gameOver = isVictory || isDefeat;
	if (gameOver)
	{
		scInput.m_towerPlacementIndex = -1;

		if (inputSystem.WasKeyJustPressed(KeyCode::Space))
		{
			NamedProperties changeStateProps;
			eventSystem.FireEvent("MissionOver", changeStateProps);
		}
		return;
	}
	
	bool isPaused = gameState.IsPaused();
	if (!isPaused)
	{
		// Game speed up/slow down
		if (inputSystem.WasKeyJustPressed(KeyCode::Plus))
		{
			float timeDilation = gameState.m_clock->GetLocalTimeDilationF();
			timeDilation *= 2.f;
			if (timeDilation > StaticGameSettings::s_maxTimeDilation)
			{
				timeDilation = StaticGameSettings::s_maxTimeDilation;
			}
			gameState.m_clock->SetLocalTimeDilation(timeDilation);
		}
		else if (inputSystem.WasKeyJustPressed(KeyCode::Minus))
		{
			float timeDilation = gameState.m_clock->GetLocalTimeDilationF();
			timeDilation *= 0.5f;
			if (timeDilation < StaticGameSettings::s_minTimeDilation)
			{
				timeDilation = StaticGameSettings::s_minTimeDilation;
			}
			gameState.m_clock->SetLocalTimeDilation(timeDilation);
		}
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
			if (!inputSystem.IsKeyDown(KeyCode::Shift))
			{
				scInput.m_towerPlacementIndex = -1;
			}
		}
		else if (inputSystem.WasMouseButtonJustPressed(1))
		{
			scInput.m_towerPlacementIndex = -1;
		}
		else if (inputSystem.WasKeyJustPressed(KeyCode::Escape))
		{
			scInput.m_towerPlacementIndex = -1;
		}
		else if (inputSystem.WasKeyJustReleased(KeyCode::Shift))
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

	// Update tower under cursor
	scInput.m_towerUnderCursor = EntityID::Invalid;
	for (auto it = context.Iterate<CPlaceable, CTransform>(); it.IsValid(); ++it)
	{
		CPlaceable const& placeable = placeableStorage[it];
		CTransform const& transform = transformStorage[it];
		AABB2 towerBounds = AABB2(transform.m_pos,static_cast<float>(placeable.m_dims.x) * 0.5f, static_cast<float>(placeable.m_dims.y) * 0.5f);
		if (towerBounds.IsPointInside(scInput.m_mouseWorldLocation))
		{
			scInput.m_towerUnderCursor = it.GetEntityID();
			break;
		}
	}

	// Sell
	if (inputSystem.WasKeyJustPressed('S') && scInput.m_towerUnderCursor != EntityID::Invalid)
	{
		if (runData.m_numSoldTowers < StaticGameSettings::s_baseSellMaximum)
		{
			factory.m_towerRemovals.push_back(TowerRemovalRequest{ scInput.m_towerUnderCursor, true });
			runData.m_numSoldTowers++;
		}
		else
		{
			FloatingTextInstance floatingTextInstance;
			floatingTextInstance.m_lifetimeSeconds = 2.f;
			floatingTextInstance.m_pos = scInput.m_mouseWorldLocation;
			floatingTextInstance.m_velocity = Vec2(0.f, 1.f);
			floatingTextInstance.m_text = "Sell limit reached!";
			floatingTextInstance.m_tint = Rgba8::Red;
			floatingText.m_floatingTextInstances.push_back(floatingTextInstance);
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool SInput::CanAffordTower(PlaceableTower const& tower, RunData const& runData)
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