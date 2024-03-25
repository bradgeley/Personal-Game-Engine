// Bradley Christensen - 2022-2023
#include "Game.h"
#include "GameCommon.h"
#include "WindowsApplication.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/FileUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Debug/DevConsole.h"
#include "Game/Guest.h"
#include "Game/GuestList.h"
#include "Game/SeatingChart.h"
#include "Game/SeatingChartGenerator.h"



//----------------------------------------------------------------------------------------------------------------------
std::string guestListFilepath = "Data/GuestList.txt";



//----------------------------------------------------------------------------------------------------------------------
void Game::Startup()
{
    m_camera = new Camera(Vec3::ZeroVector, Vec3::OneVector);     

    m_guestList = new GuestList(guestListFilepath);

    SeatingChartDefinition scDef;
    scDef.m_guestList = m_guestList;
    scDef.m_maxGuestsPerTable = 3;
    scDef.m_maxNumTables = 2; // 15 for guests, 1 for sonali/brad
    m_seatingChart = new SeatingChart(scDef);

    SeatingChartGeneratorDefinition scGenDef;
    scGenDef.m_seatingChart = m_seatingChart;
    m_generator = new SeatingChartGenerator(scGenDef);

    m_generator->Generate();
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
    if (g_input->WasKeyJustReleased(KeyCode::Escape))
    {
        g_app->Quit();
    }

    
}



//----------------------------------------------------------------------------------------------------------------------
void Game::EndFrame()
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Render() const
{
    g_renderer->BeginCameraAndWindow(m_camera, g_window);
    g_renderer->ClearScreen(Rgba8::White);
    g_renderer->EndCameraAndWindow(m_camera, g_window);
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Shutdown()
{
    delete m_camera;
    m_camera = nullptr;
}