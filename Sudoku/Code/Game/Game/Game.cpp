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
    SudokuGridConfig gridConfig;
    gridConfig.m_dims = IntVec2(9, 9);
    gridConfig.m_ruleSet.m_rules.push_back(new SudokuRuleRowOnceEach());
    gridConfig.m_ruleSet.m_rules.push_back(new SudokuRuleColumnOnceEach());
    gridConfig.m_ruleSet.m_rules.push_back(new SudokuRuleBoxOnceEach());
    gridConfig.m_startingState = Grid2D(gridConfig.m_dims, 0);
    gridConfig.m_startingState.Set(10, '6');
    gridConfig.m_solution = Grid2D(gridConfig.m_dims, 0);
    gridConfig.m_cellShading = Grid2D(gridConfig.m_dims, Rgba8::LightGray);
    gridConfig.m_bigDigitFontConstants = FontConstants(Rgba8::Black, 0.15f, 0.05f, 0.4f);

    // Setup color palette and font colors
    gridConfig.m_backgroundColor = Rgba8::LightGray;
    gridConfig.m_enteredDigitColor = Rgba8::LightBlue;
    gridConfig.m_givenDigitColor = Rgba8::Cerulean;
    gridConfig.m_cellShadingPalette.emplace_back(Rgba8(158, 1, 66));
    gridConfig.m_cellShadingPalette.emplace_back(Rgba8(213, 62, 79));
    gridConfig.m_cellShadingPalette.emplace_back(Rgba8(244, 109, 67));
    gridConfig.m_cellShadingPalette.emplace_back(Rgba8(253, 174, 97));
    gridConfig.m_cellShadingPalette.emplace_back(Rgba8(254, 224, 139));
    gridConfig.m_cellShadingPalette.emplace_back(Rgba8(230, 245, 152));
    gridConfig.m_cellShadingPalette.emplace_back(Rgba8(171, 221, 164));
    gridConfig.m_cellShadingPalette.emplace_back(Rgba8(102, 194, 165));
    gridConfig.m_cellShadingPalette.emplace_back(Rgba8(94, 79, 162));
    gridConfig.m_cellShadingPalette.emplace_back(Rgba8::LightGray);
    
    m_grid = new SudokuGrid(gridConfig);
    m_grid->Startup();
    m_player = new SudokuPlayer();
    m_player->Startup();
    m_player->BeginGame(m_grid);
}



void Game::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
    if (g_input->WasKeyJustReleased(KeyCode::Escape))
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
    m_player->BeginCamera();
    m_grid->Render();
}



void Game::Shutdown()
{
    SHUTDOWN_AND_DESTROY(m_player)
    SHUTDOWN_AND_DESTROY(m_grid)
}