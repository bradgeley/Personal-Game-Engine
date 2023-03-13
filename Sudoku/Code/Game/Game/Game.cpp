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
    gameConfig.m_dims = IntVec2(6, 6);
    gameConfig.m_ruleSet.m_rules.emplace_back(new SudokuRuleRowOnceEach());
    gameConfig.m_ruleSet.m_rules.emplace_back(new SudokuRuleColumnOnceEach());
    gameConfig.m_ruleSet.m_rules.emplace_back(new SudokuRuleBoxOnceEach());
    gameConfig.m_startingState = Grid(gameConfig.m_dims, 6);
    gameConfig.m_solution = Grid(gameConfig.m_dims, 9);
    m_grid = new SudokuGrid(gameConfig);
    m_player = new SudokuPlayer();
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

 

void Game::Render() const
{
	g_renderer->ClearScreen(Rgba8::White);
	g_renderer->BeginCamera(*(m_player->m_camera)); // reset renderer state

    m_grid->Render();
    m_player->Render();
}



void Game::Shutdown()
{
    delete m_grid;
    m_player = nullptr;
    delete m_player;
    m_grid = nullptr;
}