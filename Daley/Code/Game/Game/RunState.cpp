// Bradley Christensen - 2022-2026
#include "RunState.h"
#include "Engine/Assets/Font.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Vertex_PCU.h"
#include "Engine/Window/Window.h"



static constexpr char const* MAIN_MENU_FONT_NAME = "Data/Fonts/Gypsy.fnt";



//----------------------------------------------------------------------------------------------------------------------
RunState::RunState()
{
	m_name = "RunState";
}



//----------------------------------------------------------------------------------------------------------------------
void RunState::Enter(NamedProperties const& props)
{
	GameState::Enter(props);

	g_eventSystem->SubscribeMethod("MissionOver", this, &RunState::MissionOver);

	m_untexturedVerts = g_renderer->MakeVertexBuffer<Vertex_PCU>();
	m_textVerts = g_renderer->MakeVertexBuffer<Vertex_PCU>();

	IntVec2 windowDims = g_window->GetActualWindowResolution();
	m_camera.SetOrthoDims2D(Vec2(windowDims));

	VertexBuffer& untexturedVBO = *g_renderer->GetVertexBuffer(m_untexturedVerts);

	VertexUtils::AddVertsForAABB2(untexturedVBO, m_camera.GetOrthoBounds2D(), Rgba8::DarkGray);

	m_fontID = g_assetManager->LoadSynchronous<Font>(MAIN_MENU_FONT_NAME);
	Font const* font = g_assetManager->Get<Font>(m_fontID);
	ASSERT_OR_DIE(font != nullptr, StringUtils::StringF("Failed to load %s for RunState!", MAIN_MENU_FONT_NAME));

	// Todo: get initial placeable tower data from somewhere in xml
	m_runData = SCRunData();
	m_runData.m_seed = g_rng->Rand();
	m_runData.m_placeableTowers[0] = PlaceableTower{ "Vanilla", '1', 30.f };
	m_runData.m_placeableTowers[1] = PlaceableTower{ "Chocolate", '2', 40.f };
	m_runData.m_placeableTowers[2] = PlaceableTower{ "Strawberry", '3', 50.f };
	m_runData.m_placeableTowers[3] = PlaceableTower{ "Mint", '4', 60.f };
	m_runData.m_placeableTowers[4] = PlaceableTower{ "Wall1x1", 'W', 5.f };

	// Forest Biome first
	m_runData.m_missionGenData[0] = { "forest" };
	m_runData.m_missionGenData[1] = { "forestOpen" };
	m_runData.m_missionGenData[2] = { "forestCross" };

	// Then Desert
	m_runData.m_missionGenData[3] = { "desert" };
	m_runData.m_missionGenData[4] = { "desert" };
	m_runData.m_missionGenData[5] = { "desertCross" };

	// Then River
	m_runData.m_missionGenData[6] = { "river" };
	m_runData.m_missionGenData[7] = { "river" };
	m_runData.m_missionGenData[8] = { "riverCross" };
}



//----------------------------------------------------------------------------------------------------------------------
void RunState::Exit(NamedProperties const& props)
{
	GameState::Exit(props);

	g_eventSystem->UnsubscribeMethod("MissionOver", this, &RunState::MissionOver);

	g_renderer->ReleaseVertexBuffer(m_untexturedVerts);
	g_renderer->ReleaseVertexBuffer(m_textVerts);
}



//----------------------------------------------------------------------------------------------------------------------
void RunState::Update(float)
{
	if (g_input->WasKeyJustPressed(KeyCode::Space))
	{
		NamedProperties props;
		props.Set<Name>("state", "TowerDefense");
		props.Set<SCRunData>("runData", m_runData);
		g_eventSystem->FireEvent("PushState", props);
	}
	else if (g_input->WasKeyJustPressed(KeyCode::Escape))
	{
		NamedProperties props;
		props.Set<Name>("state", "MainMenu");
		g_eventSystem->FireEvent("ChangeState", props);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RunState::Render() const
{
	VertexBuffer& textVBO = *g_renderer->GetVertexBuffer(m_textVerts);
	textVBO.ClearVerts();

	Font const* font = g_assetManager->Get<Font>(m_fontID);
	if (font)
	{
		AABB2 cameraBounds = m_camera.GetOrthoBounds2D();
		font->AddVertsForAlignedText2D(textVBO, cameraBounds.GetCenter(), Vec2::ZeroVector, 100.f, "Run State", Rgba8::AliceBlue);
		std::string instructionText = StringUtils::StringF("Press Space to Start Mission %i\nPress ESC to return to Main Menu", m_runData.m_missionIndex + 1);
		font->AddVertsForAlignedText2D(textVBO, cameraBounds.GetBottomCenter() + Vec2(0.f, 50.f), Vec2(0.f, 1.f), 50.f, instructionText, Rgba8::Yellow);
	}

	g_renderer->BeginCameraAndWindow(&m_camera, g_window);

	g_renderer->BindTexture();
	g_renderer->BindShader();
	g_renderer->DrawVertexBuffer(m_untexturedVerts);

	if (font)
	{
		font->SetRendererState(*g_renderer);
		g_renderer->DrawVertexBuffer(m_textVerts);
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool RunState::MissionOver(NamedProperties& props)
{
	m_runData = props.Get<SCRunData>("runData", SCRunData());

	if (m_runData.m_health <= 0.f)
	{
		NamedProperties popStateProps;
		popStateProps.Set<Name>("state", "MainMenu");
		popStateProps.Set<bool>("wipe", true);
		g_eventSystem->FireEvent("ChangeState", popStateProps);
	}
	else
	{
		m_runData.m_missionIndex++;
		if (m_runData.m_missionIndex == StaticGameSettings::s_numMissionsForVictory)
		{
			NamedProperties changeStateProps;
			changeStateProps.Set<Name>("state", "GameOver");
			g_eventSystem->FireEvent("ChangeState", changeStateProps);
		}
		else
		{
			NamedProperties popStateProps;
			popStateProps.Set<Name>("state", "RunState");
			g_eventSystem->FireEvent("PopState", popStateProps);
		}
	}

	return false;
}
