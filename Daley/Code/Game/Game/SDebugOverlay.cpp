// Bradley Christensen - 2022-2026
#include "SDebugOverlay.h"
#include "CAbility.h"
#include "CCollision.h"
#include "CEntityName.h"
#include "CHealth.h"
#include "CMovement.h"
#include "CProjectile.h"
#include "CPlaceable.h"
#include "CTags.h"
#include "CTime.h"
#include "CTransform.h"
#include "SCCamera.h"
#include "SCDebug.h"
#include "SCFlowField.h"
#include "SCInputSystem.h"
#include "SCRenderer.h"
#include "SCRunData.h"
#include "SCWaves.h"
#include "SGoal.h"
#include "Engine/Assets/Font.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"




//----------------------------------------------------------------------------------------------------------------------
struct DebugOverlayLine
{
	std::string m_text;
	Rgba8 m_color = Rgba8::White;
};



//----------------------------------------------------------------------------------------------------------------------
void SDebugOverlay::Startup()
{
    AddWriteAllDependencies();

	DevConsoleUtils::AddDevConsoleCommand("ToggleDebugOverlay", &SDebugOverlay::ToggleDebugOverlay);
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugOverlay::Shutdown() const
{
	DevConsoleUtils::RemoveDevConsoleCommand("ToggleDebugOverlay", &SDebugOverlay::ToggleDebugOverlay);
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugOverlay::Run(SystemContext const& context) const
{
	// Write Dependencies
	SCDebug& scDebug = context.GetSingleton<SCDebug>();
	Renderer& renderer = *context.GetSingleton<SCRenderer>().GetRenderer();

	// Read Dependencies
	SCCamera const& scCamera = context.GetSingletonConst<SCCamera>();
	SCWaves const& waves = context.GetSingletonConst<SCWaves>();
	SCRunData const& scRunData = context.GetSingletonConst<SCRunData>();
	RunData const& runData = *scRunData.m_data;
	auto& collisionStorage = context.GetArrayStorageConst<CCollision>();
	auto& tagsStorage = context.GetArrayStorageConst<CTags>();
	auto& transStorage = context.GetArrayStorageConst<CTransform>();
	auto& debugStorage = context.GetArrayStorageConst<CEntityName>();
	auto& abilityStorage = context.GetMapStorageConst<CAbility>();
	auto& healthStorage = context.GetArrayStorageConst<CHealth>();
	auto& timeStorage = context.GetArrayStorageConst<CTime>();
	auto& movementStorage = context.GetArrayStorageConst<CMovement>();
	auto& projectileStorage = context.GetMapStorageConst<CProjectile>();
	SCInputSystem const& scInput = context.GetSingletonConst<SCInputSystem>();
	InputSystem const& input = *scInput.GetInputSystem();

	BitMask collisionBit = context.GetComponentBitMask<CCollision>();

	if (input.IsKeyDown(KeyCode::Ctrl) && input.WasKeyJustPressed('D'))
	{
		scDebug.m_debugOverlayEnabled = !scDebug.m_debugOverlayEnabled;
	}

	if (!scDebug.m_debugOverlayEnabled)
	{
		return;
	}

	VertexBuffer& untexVerts = *renderer.GetVertexBuffer(scDebug.m_frameUntexVerts);
	VertexBuffer& fontVerts = *renderer.GetVertexBuffer(scDebug.m_frameDefaultFontVerts);
	Font const* font = renderer.GetDefaultFont();
	if (!font)
	{
		return;
	}

	Camera const& screenCamera = scCamera.m_uiCamera;
	AABB2 screenBounds = screenCamera.GetOrthoBounds2D();

	// Add transparent black background
	VertexUtils::AddVertsForAABB2(untexVerts, screenBounds, Rgba8(0, 0, 0, 120), AABB2::ZeroToOne);

	float const overlayTextHeight = 25.f;
	float const spacing = 5.f;
	float const leftSidePadding = 200.f;
	float const topPadding = 110.f;
	float const spaceBetweenLines = overlayTextHeight + spacing;
	int const numColumns = 2;

	// Every line shown in the overlay's columns is added through this one lambda
	std::vector<DebugOverlayLine> debugOverlayLines;
	auto addDebugOverlayLine = [&](std::string const& text, Rgba8 const& color = Rgba8::White)
	{
		debugOverlayLines.push_back({ text, color });
	};
	auto addHeader = [&](std::string const& text)
	{
		addDebugOverlayLine("");
		addDebugOverlayLine(text, Rgba8::Yellow);
	};

	// General
	addDebugOverlayLine("Debug Overlay Enabled", Rgba8::Yellow);
	addDebugOverlayLine(StringUtils::StringF("Total Entities: %d", context.NumEntities()));
	addDebugOverlayLine(StringUtils::StringF("Remaining Enemies: %d", waves.m_remainingEnemies));
	addDebugOverlayLine(StringUtils::StringF("God Mode: %s", scDebug.m_godMode ? "Enabled" : "Disabled"));

	// Current Mission
	addHeader("Current Mission");
	addDebugOverlayLine(StringUtils::StringF("Map: %s", runData.m_missionGenData[runData.m_missionIndex].m_mapName.ToCStr()));
	addDebugOverlayLine(StringUtils::StringF("Run Seed: %d", runData.m_seed));
	addDebugOverlayLine(StringUtils::StringF("Mission Seed: %d", runData.m_seed + runData.m_missionIndex));
	addDebugOverlayLine(StringUtils::StringF("Current Mission: %d/%d", runData.m_missionIndex + 1, (int) runData.m_missionGenData.size()));

	// Waves
	addHeader("Waves");
	addDebugOverlayLine(StringUtils::StringF("Waves: %s", waves.m_wavesStarted ? (waves.m_wavesFinished ? "Finished" : "In Progress") : "Not Started"));
	addDebugOverlayLine(StringUtils::StringF("Current Wave: %d/%d", waves.m_currentWaveIndex, (int) waves.m_waves.size()));
	addDebugOverlayLine(StringUtils::StringF("Time Until Next Wave: %.1f", waves.m_waveTimer.GetRemainingSeconds()), Rgba8::Yellow);
	for (ActiveWaveStream const& activeStream : waves.m_activeStreams)
	{
		addDebugOverlayLine(StringUtils::StringF("Active Stream: %s (%d/%d)", activeStream.m_entityStream.m_entityName.ToCStr(), activeStream.m_numSpawned, activeStream.m_entityStream.m_numEntities));
	}

	// Static / Generated Run Data
	addHeader("Run Data");
	addDebugOverlayLine(StringUtils::StringF("Num Run Modifier Defs: %d", (int) runData.m_runModifierPool.m_runModifierDefs.size()));
	addDebugOverlayLine(StringUtils::StringF("Seed: %d", runData.m_seed));
	addDebugOverlayLine(StringUtils::StringF("Mode: %s", runData.m_mode == GameMode::Endless ? "Endless" : "Journey"));
	addDebugOverlayLine(StringUtils::StringF("Num Missions: %d", (int) runData.m_missionGenData.size()));
	for (size_t i = 0; i < runData.m_missionGenData.size(); ++i)
	{
		MissionGenData const& missionGenData = runData.m_missionGenData[i];
		addDebugOverlayLine(StringUtils::StringF("Mission %d: %s (Biome: %s)", (int) i, missionGenData.m_mapName.ToCStr(), missionGenData.m_biomeName.ToCStr()));
	}

	// Non-Resettable Data
	addHeader("Non-Resettable Data");
	ExperienceLevelData const levelData = RunData::GetLevelData(runData.m_experience);
	addDebugOverlayLine(StringUtils::StringF("Mission Index: %d", runData.m_missionIndex));
	addDebugOverlayLine(StringUtils::StringF("Experience: %llu", runData.m_experience));
	addDebugOverlayLine(StringUtils::StringF("Fractional Experience: %.2f", runData.m_fractionalExperience));
	addDebugOverlayLine(StringUtils::StringF("Level: %d%s (%.1f%%, %llu/%llu)", levelData.m_level, levelData.m_isMaxLevel ? " (Max)" : "", levelData.m_percentIntoLevel * 100.f, levelData.m_experienceIntoLevel, levelData.m_experienceNeededForNextLevel));
	addDebugOverlayLine(StringUtils::StringF("Modifier Choices Remaining: %d", runData.m_numModifierChoicesRemaining));
	addDebugOverlayLine(StringUtils::StringF("Modifier Choices Completed: %d", runData.m_numModifierChoicesCompleted));
	addDebugOverlayLine(StringUtils::StringF("Active Run Modifiers: %d", (int) runData.m_activeRunModifiers.size()));
	for (size_t i = 0; i < runData.m_placeableTowers.size(); ++i)
	{
		PlaceableTower const& tower = runData.m_placeableTowers[i];
		addDebugOverlayLine(StringUtils::StringF("Placeable Tower %d: %s / %s [%c] Cost: %.1f (Base: %.1f)", (int) i, tower.m_towerName.ToCStr(), tower.m_flavorName.ToCStr(), tower.m_hotkey, tower.m_cost, tower.m_baseCost));
	}
	for (size_t i = 0; i < runData.m_towerPlacementKeyBindings.size(); ++i)
	{
		addDebugOverlayLine(StringUtils::StringF("Placement Key Binding %d: %c", (int) i, (char) runData.m_towerPlacementKeyBindings[i]));
	}

	// Resettable Data
	addHeader("Resettable Data");
	addDebugOverlayLine(StringUtils::StringF("Gold: %.1f", runData.GetAttribute("Gold", 0.f)));
	addDebugOverlayLine(StringUtils::StringF("Health: %.1f", runData.GetAttribute("Health", 0.f)));
	addDebugOverlayLine(StringUtils::StringF("Income Interval Remaining: %.1f", runData.m_incomeIntervalSecondsRemaining));
	addDebugOverlayLine(StringUtils::StringF("Sells Used: %d", static_cast<int>(runData.GetAttribute("SellsUsed", 0.f))));
	addDebugOverlayLine(StringUtils::StringF("Num Active Modifier Choices: %d", runData.m_numActiveModifierChoices));

	// Modifiable Data
	addHeader("Modifiable Data");
	addDebugOverlayLine(StringUtils::StringF("Gold Gain Multiplier: %.2f", runData.GetAttribute("GoldMulti", 0.f)));
	addDebugOverlayLine(StringUtils::StringF("Experience Multiplier: %.2f", runData.GetAttribute("ExpMulti", 0.f)));
	addDebugOverlayLine(StringUtils::StringF("Income: %.1f", runData.GetAttribute("Income", 0.f)));
	addDebugOverlayLine(StringUtils::StringF("Credit Limit: %.1f", runData.GetAttribute("CreditLimit", 0.f)));
	addDebugOverlayLine(StringUtils::StringF("Savings Interest Rate: %.3f", runData.GetAttribute("SavingsInterestRate", 0.f)));
	addDebugOverlayLine(StringUtils::StringF("Debt Interest Rate: %.3f", runData.GetAttribute("DebtInterestRate", 0.f)));
	addDebugOverlayLine(StringUtils::StringF("Income Interval: %.1f", runData.GetAttribute("IncomeInterval", 0.f)));
	addDebugOverlayLine(StringUtils::StringF("Max Health: %.1f", runData.GetAttribute("MaxHealth", 0.f)));
	addDebugOverlayLine(StringUtils::StringF("Health Regen: %.2f", runData.GetAttribute("HealthRegen", 0.f)));
	addDebugOverlayLine(StringUtils::StringF("Flavor Limit: %d", static_cast<int>(runData.GetAttribute("FlavorLimit", 0.f))));
	addDebugOverlayLine(StringUtils::StringF("Swirl Limit: %d", static_cast<int>(runData.GetAttribute("SwirlLimit", 0.f))));
	addDebugOverlayLine(StringUtils::StringF("Cost Ramp Per Tower: %.3f", runData.GetAttribute("TowerCostRamp", 0.f)));
	addDebugOverlayLine(StringUtils::StringF("Sell Refund Ratio: %.2f", runData.GetAttribute("SellRefundRatio", 0.f)));
	addDebugOverlayLine(StringUtils::StringF("Max Sells Per Mission: %d", static_cast<int>(runData.GetAttribute("SellLimit", 0.f))));

	// Persistent / Transient State
	addHeader("Persistent / Transient State");
	addDebugOverlayLine(StringUtils::StringF("Current Time Dilation: %.2f", runData.m_currentTimeDilation));
	addDebugOverlayLine(StringUtils::StringF("Wants Modifier Choice: %s", runData.m_wantsModifierChoice ? "true" : "false"));
	addDebugOverlayLine(StringUtils::StringF("Needs Modifier Recalculation: %s", runData.m_needsModifierRecalculation ? "true" : "false"));

	// Lay out all lines evenly across the columns, spacing columns by the width of their widest line
	float const spaceBetweenColumns = 50.f;
	Vec2 const linesTopLeft = screenBounds.GetTopLeft() + Vec2(leftSidePadding, -topPadding);
	int const numLines = (int) debugOverlayLines.size();
	int const numLinesPerColumn = (numLines + numColumns - 1) / numColumns;

	// Find the widest line in each column
	std::array<float, numColumns> columnWidths = {};
	for (int lineIndex = 0; lineIndex < numLines; ++lineIndex)
	{
		int const columnIndex = lineIndex / numLinesPerColumn;
		float const lineWidth = font->GetLineWidth(overlayTextHeight, debugOverlayLines[lineIndex].m_text);
		columnWidths[columnIndex] = MathUtils::Max(columnWidths[columnIndex], lineWidth);
	}

	// Each column starts after the previous column's widest line, plus spacing
	std::array<float, numColumns> columnStartXs = {};
	for (int columnIndex = 1; columnIndex < numColumns; ++columnIndex)
	{
		columnStartXs[columnIndex] = columnStartXs[columnIndex - 1] + columnWidths[columnIndex - 1] + spaceBetweenColumns;
	}

	for (int lineIndex = 0; lineIndex < numLines; ++lineIndex)
	{
		int const columnIndex = lineIndex / numLinesPerColumn;
		int const rowIndex = lineIndex % numLinesPerColumn;
		Vec2 const linePos = linesTopLeft + Vec2(columnStartXs[columnIndex], -spaceBetweenLines * (float) rowIndex);
		font->AddVertsForAlignedText2D(fontVerts, linePos, Vec2(1.f, -1.f), overlayTextHeight, debugOverlayLines[lineIndex].m_text, debugOverlayLines[lineIndex].m_color);
	}
	
	// Show ability information for hovered entity
	for (auto it = context.Iterate<CTags, CTransform, CEntityName>(); it.IsValid(); ++it)
	{
		CTags const& tags = tagsStorage[it];
		bool isTower = tags.HasTag("Tower");
		bool isObstacle = tags.HasTag("obstacle");
		bool isEnemy = tags.HasTag("enemy");
		bool isProj = tags.HasTag("projectile");
		bool isDebugabble = isTower || isObstacle || isEnemy || isProj;
		if (isDebugabble)
		{
			CTransform const& transform = transStorage[it];
			CCollision const* collision = context.HasComponents(it.GetEntityID(), collisionBit) ? &collisionStorage[it] : nullptr;
			float radius = 1.f;
			if (collision)
			{
				radius = collision->m_radius;
			}
			else
			{
				CPlaceable const* placeable = context.GetComponent<CPlaceable>(it.GetEntityID());
				if (placeable)
				{
					radius = static_cast<float>(placeable->m_dims.x) * 0.5f;
				}
			}

			if (scInput.m_mouseWorldLocation.GetDistanceSquaredTo(transform.m_pos) < (radius * radius))
			{
				CEntityName const& debug = debugStorage[it];
				Vec2 screenPos = scCamera.m_worldCamera.WorldToScreenRelativeOrtho(transform.m_pos + Vec2(radius, radius)) * screenCamera.GetOrthoDimensions2D();
				Vec2 cardMins = screenPos;

				EntityDebugContext debugContext(it.GetEntityID(), context);

				if (context.HasComponent<CAbility>(it.GetEntityID()))
				{
					CAbility const& abilityComp = abilityStorage[it];
					abilityComp.AppendDebugString(debugContext);
				}

				if (context.HasComponent<CHealth>(it.GetEntityID()))
				{
					CHealth const& health = healthStorage[it];
 					health.AppendDebugString(debugContext);
				}
				if (context.HasComponent<CMovement>(it.GetEntityID()))
				{
					CMovement const& movement = movementStorage[it];
					movement.AppendDebugString(debugContext);
				}
				if (context.HasComponent<CTime>(it.GetEntityID()))
				{
					CTime const& time = timeStorage[it];
					time.AppendDebugString(debugContext.m_debugString);
				}
				if (context.HasComponent<CProjectile>(it.GetEntityID()))
				{
					CProjectile const& proj = projectileStorage[it];
					proj.AppendDebugString(debugContext);
				}

				float damage = SGoal::GetDamageToPlayerByTags(tags);
				if (damage > 0.f)
				{
					debugContext.m_debugString += StringUtils::StringF("Player Damage: %.1f\n", damage);
				}

				tags.AppendDebugString(debugContext.m_debugString);

				if (debugContext.m_debugString.empty())
				{
					debugContext.m_debugString = "No Debug Info Available";
				}

				float titleLineHeight = 33.f;
				float textLineHeight = 25.f;
				float textLineSpacingRatio = 0.167f;
				float infoCardEdgeWidth = 5.f;
				float infoCardTitleSpacing = 20.f;
				Vec2 minCardDims = Vec2(300.f, 300.f);
				StringUtils::TrimTrailingWhitespace(debugContext.m_debugString);
				Strings splitLines = StringUtils::SplitStringOnDelimiter(debugContext.m_debugString, '\n');
				int numSpitLines = (int) splitLines.size();
				float textHeight = titleLineHeight + infoCardTitleSpacing + textLineHeight * (float) numSpitLines + textLineSpacingRatio * textLineHeight* (float) (numSpitLines - 1);
				float totalHeight = textHeight + infoCardEdgeWidth * 2.f;
				float cardHeight = MathUtils::Max(minCardDims.y, totalHeight);
				float textStartHeightOffset = -infoCardEdgeWidth - titleLineHeight - infoCardTitleSpacing;

				int longestLineIndex = 0;
				float longestLineWidth = 0.f;

				for (int lineIndex = 0; lineIndex < numSpitLines; ++lineIndex)
				{
					float width = font->GetLineWidth(textLineHeight, splitLines[lineIndex]);
					if (width > longestLineWidth)
					{
						longestLineWidth = width;
						longestLineIndex = lineIndex;
					}
				}

				float titleWidth = font->GetLineWidth(titleLineHeight, debug.m_defName.ToString());
				float cardWidth = MathUtils::Max(minCardDims.x, longestLineWidth + infoCardEdgeWidth * 2.f);
				cardWidth = MathUtils::Max(cardWidth, titleWidth + infoCardEdgeWidth * 2.f);
				Vec2 cardDims = Vec2(cardWidth, cardHeight);
				AABB2 informationCardBounds = AABB2(cardMins, cardMins + cardDims);

				// Clamp card to screen
				if (informationCardBounds.maxs.x > screenBounds.maxs.x)
				{
					float offset = informationCardBounds.maxs.x - screenBounds.maxs.x;
					informationCardBounds.Translate(Vec2(-offset, 0.f));
				}
				else if (informationCardBounds.mins.x < screenBounds.mins.x)
				{
					float offset = screenBounds.mins.x - informationCardBounds.mins.x;
					informationCardBounds.Translate(Vec2(offset, 0.f));
				}
				if (informationCardBounds.maxs.y > screenBounds.maxs.y)
				{
					float offset = informationCardBounds.maxs.y - screenBounds.maxs.y;
					informationCardBounds.Translate(Vec2(0.f, -offset));
				}
				else if (informationCardBounds.mins.y < screenBounds.mins.y)
				{
					float offset = screenBounds.mins.y - informationCardBounds.mins.y;
					informationCardBounds.Translate(Vec2(0.f, offset));
				}

				// Background
				VertexUtils::AddVertsForAABB2(untexVerts, informationCardBounds, Rgba8::DarkGray);

				// Background Outline
				VertexUtils::AddVertsForWireBox2D(untexVerts, informationCardBounds, infoCardEdgeWidth, Rgba8::Black);

				// Title (def + entity ID)
				std::string title = debug.m_defName.ToString();
				title += StringUtils::StringF(" (ID: %d)", it.GetEntityID());
				font->AddVertsForAlignedText2D(fontVerts, informationCardBounds.GetTopLeft() + Vec2(infoCardEdgeWidth, -infoCardEdgeWidth), Vec2(1.f, -1.f), titleLineHeight, title, Rgba8::White);

				// Debug Information
				Vec2 debugInfoTextDims = font->GetTextDims(textLineHeight, textLineSpacingRatio, debugContext.m_debugString);
				Vec2 offset = Vec2::ZeroVector;
				offset.x = -(informationCardBounds.GetWidth() - debugInfoTextDims.x) + infoCardEdgeWidth;
				offset.y = (informationCardBounds.GetHeight() - debugInfoTextDims.y) + textStartHeightOffset;
				font->AddVertsForAlignedText2D(fontVerts, informationCardBounds.GetBottomRight() + offset, Vec2(-1.f, 1.f), textLineHeight, debugContext.m_debugString, Rgba8::White, textLineSpacingRatio);
				break;
			}
		}
	}

	renderer.BeginCamera(&screenCamera);

	// Render->clear debug verts 
	renderer.BindTexture();
	renderer.BindShader();
	renderer.DrawVertexBuffer(untexVerts);
	untexVerts.ClearVerts();

	// Render->clear debug text verts 
	font->SetRendererState(*g_renderer);
	renderer.DrawVertexBuffer(fontVerts);
	fontVerts.ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugOverlay::ToggleDebugOverlay(NamedProperties&)
{
	SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
	scDebug.m_debugOverlayEnabled = !scDebug.m_debugOverlayEnabled;
	return false;
}