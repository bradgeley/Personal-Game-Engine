// Bradley Christensen - 2022-2023
#include "Game.h"
#include "GameCommon.h"
#include "WindowsApplication.h"
#include "Engine/Input/InputSystem.h"
#include "Engine\Core\EngineCommon.h"
#include "Game/SudokuGrid.h"
#include "Game/SudokuPlayer.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Rgba8.h"




void Game::Startup()
{

}



void Game::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
    if (g_input->WasKeyJustReleased(KeyCode::ESCAPE))
    {
        g_app->Quit();
    }

    m_player->Update(deltaSeconds);
    m_grid->Update(deltaSeconds);
}

 

void Game::Render() const
{
	g_renderer->ClearScreen(Rgba8::White);
	g_renderer->BeginCamera(*(m_player->m_camera)); // reset renderer state

    m_grid->Render();
    m_player->Render();
}



void Game::Shutdown()
{

}