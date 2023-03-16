// Bradley Christensen - 2022-2023
#include "Game.h"
#include "GameCommon.h"
#include "WindowsApplication.h"
#include "Game/SudokuGrid.h"
#include "Game/SudokuPlayer.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Core/EngineCommon.h"



void Game::Startup()
{
    SudokuGridConfig gameConfig;
    gameConfig.m_dims = IntVec2(9, 9);
    gameConfig.m_ruleSet.m_rules.emplace_back(new SudokuRuleRowOnceEach());
    gameConfig.m_ruleSet.m_rules.emplace_back(new SudokuRuleColumnOnceEach());
    gameConfig.m_ruleSet.m_rules.emplace_back(new SudokuRuleBoxOnceEach());
    gameConfig.m_startingState = Grid2D(gameConfig.m_dims, (int) 0);
    gameConfig.m_startingState.Set(10, '6');
    gameConfig.m_solution = Grid2D(gameConfig.m_dims, (int) 0);
    m_grid = new SudokuGrid(gameConfig);
    m_grid->Startup();
    m_player = new SudokuPlayer();
    m_player->Startup();
    m_player->BeginGame(m_grid);
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



void Game::EndFrame()
{
    m_player->EndFrame();
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
    SHUTDOWN_AND_DESTROY(m_player)
    SHUTDOWN_AND_DESTROY(m_grid)
}