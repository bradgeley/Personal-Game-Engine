// Bradley Christensen - 2022-2026
#include "RunState.h"
#include "Engine/Assets/Font.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Input/InputSystem.h"
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
void RunState::Enter()
{
	GameState::Enter();

	g_eventSystem->SubscribeMethod("MissionOver", this, &RunState::MissionOver);

	m_untexturedVerts = g_renderer->MakeVertexBuffer<Vertex_PCU>();
	m_textVerts = g_renderer->MakeVertexBuffer<Vertex_PCU>();

	IntVec2 windowDims = g_window->GetActualWindowResolution();
	m_camera.SetOrthoDims2D(Vec2(windowDims));

	VertexBuffer& textVBO = *g_renderer->GetVertexBuffer(m_textVerts);
	VertexBuffer& untexturedVBO = *g_renderer->GetVertexBuffer(m_untexturedVerts);

	VertexUtils::AddVertsForAABB2(untexturedVBO, m_camera.GetOrthoBounds2D(), Rgba8::DarkGray);

	m_fontID = g_assetManager->LoadSynchronous<Font>(MAIN_MENU_FONT_NAME);
	Font const* font = g_assetManager->Get<Font>(m_fontID);
	ASSERT_OR_DIE(font != nullptr, StringUtils::StringF("Failed to load %s for RunState!", MAIN_MENU_FONT_NAME));

	if (font)
	{
		font->AddVertsForAlignedText2D(textVBO, Vec2::ZeroVector, Vec2::ZeroVector, 100.f, "Run State", Rgba8::AliceBlue);
		font->AddVertsForAlignedText2D(textVBO, Vec2(0.f, -500.f), Vec2::ZeroVector, 50.f, "Press Space to Start Mission\nPress ESC to return to Main Menu", Rgba8::Yellow);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RunState::Exit()
{
	GameState::Exit();

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
	g_renderer->BeginCameraAndWindow(&m_camera, g_window);

	g_renderer->BindTexture();
	g_renderer->BindShader();
	g_renderer->DrawVertexBuffer(m_untexturedVerts);

	Font const* font = g_assetManager->Get<Font>(m_fontID);
	font->SetRendererState(*g_renderer);
	g_renderer->DrawVertexBuffer(m_textVerts);
}



//----------------------------------------------------------------------------------------------------------------------
bool RunState::MissionOver(NamedProperties& props)
{
	SCRunData runData = props.Get<SCRunData>("runData", SCRunData());

	if (runData.m_currentHealth <= 0.f)
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

	return false;
}
