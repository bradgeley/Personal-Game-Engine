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
#include "SCWindow.h"
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
#include "Engine/Window/Window.h"



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
	SCCamera const& worldCamera = context.GetSingletonConst<SCCamera>();
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
	Window const& window = *context.GetSingletonConst<SCWindow>().GetWindow();

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

	Camera screenCamera;
	IntVec2 resolution = window.GetRenderResolution();
	AABB2 screenBounds = AABB2(0.f, 0.f, (float) resolution.x, (float) resolution.y);
	screenCamera.SetOrthoBounds2D(screenBounds);

	// Add transparent black background
	VertexUtils::AddVertsForAABB2(untexVerts, screenBounds, Rgba8(0, 0, 0, 120), AABB2::ZeroToOne);

	float currentHeightOffset = -10.f;
	float overlayTextHeight = 25.f;
	float spacing = 5.f;
	float leftSidePadding = 200.f;
	float spaceBetweenLines = overlayTextHeight + spacing;


	Vec2 topLeft = screenBounds.GetTopLeft();
	currentHeightOffset -= 100.f;
	font->AddVertsForAlignedText2D(fontVerts, topLeft + Vec2(leftSidePadding, currentHeightOffset), Vec2(1.f, -1.f), overlayTextHeight, "Debug Overlay Enabled", Rgba8::White);

	currentHeightOffset -= spaceBetweenLines;
	int totalEntities = context.NumEntities();
	font->AddVertsForAlignedText2D(fontVerts, topLeft + Vec2(leftSidePadding, currentHeightOffset), Vec2(1.f, -1.f), overlayTextHeight, StringUtils::StringF("Total Entities: %d", totalEntities), Rgba8::White);

	currentHeightOffset -= spaceBetweenLines;
	int numEnemies = waves.m_remainingEnemies;
	font->AddVertsForAlignedText2D(fontVerts, topLeft + Vec2(leftSidePadding, currentHeightOffset), Vec2(1.f, -1.f), overlayTextHeight, StringUtils::StringF("Remaining Enemies: %d", numEnemies), Rgba8::White);

	currentHeightOffset -= spaceBetweenLines;
	Name mapName = runData.m_missionGenData[runData.m_missionIndex].m_mapName;
	font->AddVertsForAlignedText2D(fontVerts, topLeft + Vec2(leftSidePadding, currentHeightOffset), Vec2(1.f, -1.f), overlayTextHeight, StringUtils::StringF("Map: %s", mapName.ToCStr()), Rgba8::White);

	currentHeightOffset -= spaceBetweenLines;
	font->AddVertsForAlignedText2D(fontVerts, topLeft + Vec2(leftSidePadding, currentHeightOffset), Vec2(1.f, -1.f), overlayTextHeight, StringUtils::StringF("Run Seed: %d", runData.m_seed), Rgba8::White);

	currentHeightOffset -= spaceBetweenLines;
	font->AddVertsForAlignedText2D(fontVerts, topLeft + Vec2(leftSidePadding, currentHeightOffset), Vec2(1.f, -1.f), overlayTextHeight, StringUtils::StringF("Mission Seed: %d", runData.m_seed + runData.m_missionIndex), Rgba8::White);

	currentHeightOffset -= spaceBetweenLines;
	font->AddVertsForAlignedText2D(fontVerts, topLeft + Vec2(leftSidePadding, currentHeightOffset), Vec2(1.f, -1.f), overlayTextHeight, StringUtils::StringF("Current Mission: %d/%d", runData.m_missionIndex + 1, runData.m_missionGenData.size()), Rgba8::White);

	currentHeightOffset -= spaceBetweenLines;
	bool isGodMode = scDebug.m_godMode;
	font->AddVertsForAlignedText2D(fontVerts, topLeft + Vec2(leftSidePadding, currentHeightOffset), Vec2(1.f, -1.f), overlayTextHeight, StringUtils::StringF("God Mode: %s", isGodMode ? "Enabled" : "Disabled"), Rgba8::White);

	// Wave spawner info
	Vec2 wavesTopLeft = screenBounds.maxs - Vec2(350.f, 10.f);
	font->AddVertsForAlignedText2D(fontVerts, wavesTopLeft, Vec2(1.f, -1.f), overlayTextHeight, StringUtils::StringF("Waves: %s", waves.m_wavesStarted ? (waves.m_wavesFinished ? "Finished" : "In Progress") : "Not Started"), Rgba8::White);
	font->AddVertsForAlignedText2D(fontVerts, wavesTopLeft - Vec2(0.f, 30.f), Vec2(1.f, -1.f), overlayTextHeight, StringUtils::StringF("Current Wave: %d/%d", waves.m_currentWaveIndex, waves.m_waves.size()), Rgba8::White);
	font->AddVertsForAlignedText2D(fontVerts, wavesTopLeft - Vec2(0.f, 60.f), Vec2(1.f, -1.f), overlayTextHeight, StringUtils::StringF("Time Until Next Wave: %.1f", waves.m_waveTimer.GetRemainingSeconds()), Rgba8::Yellow);
	for (int streamIndex = 0; streamIndex < (int) waves.m_activeStreams.size(); ++streamIndex)
	{
		ActiveWaveStream const& activeStream = waves.m_activeStreams[streamIndex];
		font->AddVertsForAlignedText2D(fontVerts, wavesTopLeft - Vec2(0.f, 90.f + 30.f * (float) streamIndex), Vec2(1.f, -1.f), overlayTextHeight, StringUtils::StringF("Active Stream: %s (%d/%d)", activeStream.m_entityStream.m_entityName.ToCStr(), activeStream.m_numSpawned, activeStream.m_entityStream.m_numEntities), Rgba8::White);
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
				Vec2 screenPos = worldCamera.m_worldCamera.WorldToScreenRelativeOrtho(transform.m_pos + Vec2(radius, radius)) * screenCamera.GetOrthoDimensions2D();
				Vec2 cardMins = screenPos;

				EntityDebugContext debugContext(it.GetEntityID(), context);

				if (context.HasComponent<CAbility>(it.GetEntityID()))
				{
					CAbility const& abilityComp = abilityStorage[it];
					for (auto& ability : abilityComp.m_abilities)
					{
						ability->AppendDebugString(debugContext);
						if (abilityComp.m_abilities.size() > 1)
						{
							debugContext.m_debugString += "-----------------------------------\n";
						}
					}
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
				Strings lines = StringUtils::SplitStringOnDelimiter(debugContext.m_debugString, '\n');
				int numLines = (int) lines.size();
				float textHeight = titleLineHeight + infoCardTitleSpacing + textLineHeight * (float) numLines + textLineSpacingRatio * textLineHeight* (float) (numLines - 1);
				float totalHeight = textHeight + infoCardEdgeWidth * 2.f;
				float cardHeight = MathUtils::Max(minCardDims.y, totalHeight);
				float textStartHeightOffset = -infoCardEdgeWidth - titleLineHeight - infoCardTitleSpacing;

				int longestLineIndex = 0;
				float longestLineWidth = 0.f;

				for (int lineIndex = 0; lineIndex < numLines; ++lineIndex)
				{
					float width = font->GetLineWidth(textLineHeight, lines[lineIndex]);
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
