// Bradley Christensen - 2022-2026
#include "MainMenuState.h"
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
MainMenuState::MainMenuState()
{
	m_name = "MainMenu";
}



//----------------------------------------------------------------------------------------------------------------------
void MainMenuState::Enter(NamedProperties const& props)
{
	GameState::Enter(props);

	m_untexturedVerts = g_renderer->MakeVertexBuffer<Vertex_PCU>();
	m_textVerts = g_renderer->MakeVertexBuffer<Vertex_PCU>();

	IntVec2 windowDims = g_window->GetActualWindowResolution();
	m_camera.SetOrthoDims2D(Vec2(windowDims));

	VertexBuffer& textVBO = *g_renderer->GetVertexBuffer(m_textVerts);
	VertexBuffer& untexturedVBO = *g_renderer->GetVertexBuffer(m_untexturedVerts);

	VertexUtils::AddVertsForAABB2(untexturedVBO, m_camera.GetOrthoBounds2D(), Rgba8::Gray);

	m_fontID = g_assetManager->LoadSynchronous<Font>(MAIN_MENU_FONT_NAME);
	Font const* font = g_assetManager->Get<Font>(m_fontID);
	ASSERT_OR_DIE(font != nullptr, StringUtils::StringF("Failed to load %s for MainMenu!", MAIN_MENU_FONT_NAME));

	if (font)
	{
		font->AddVertsForAlignedText2D(textVBO, Vec2::ZeroVector, Vec2::ZeroVector, 100.f, "Main Menu", Rgba8::AliceBlue);
		font->AddVertsForAlignedText2D(textVBO, Vec2(0.f, -500.f), Vec2::ZeroVector, 50.f, "Press Space to Start Run\nPress ESC to quit", Rgba8::Yellow);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void MainMenuState::Exit(NamedProperties const& props)
{
	GameState::Exit(props);

	g_renderer->ReleaseVertexBuffer(m_untexturedVerts);
	g_renderer->ReleaseVertexBuffer(m_textVerts);
}



//----------------------------------------------------------------------------------------------------------------------
void MainMenuState::Update(float)
{
	if (g_input->WasKeyJustPressed(KeyCode::Escape))
	{
		g_eventSystem->FireEvent("Quit");
	}
	else if (g_input->WasKeyJustPressed(KeyCode::Space))
	{
		NamedProperties props;
		props.Set<Name>("state", "Run");
		g_eventSystem->FireEvent("ChangeState", props);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void MainMenuState::Render() const
{
	g_renderer->BeginCameraAndWindow(&m_camera, g_window);

	g_renderer->BindTexture();
	g_renderer->BindShader();
	g_renderer->DrawVertexBuffer(m_untexturedVerts);

	Font const* font = g_assetManager->Get<Font>(m_fontID);
	font->SetRendererState(*g_renderer);
	g_renderer->DrawVertexBuffer(m_textVerts);
}
