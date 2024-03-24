// Bradley Christensen - 2022-2023
#include "Game.h"
#include "GameCommon.h"
#include "WindowsApplication.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Core/FileUtils.h"
#include "Engine/Debug/DevConsole.h"



//----------------------------------------------------------------------------------------------------------------------
std::string filepath = "Data/GuestList.txt";



//----------------------------------------------------------------------------------------------------------------------
void Game::Startup()
{
    m_camera = new Camera(Vec3::ZeroVector, Vec3::OneVector);

    g_devCon
}



void Game::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
    if (g_input->WasKeyJustReleased(KeyCode::Escape))
    {
        g_app->Quit();
    }

    
}



void Game::EndFrame()
{
    
}



void Game::Render() const
{
    g_renderer->BeginCameraAndWindow(m_camera, g_window);
    g_renderer->ClearScreen(Rgba8::White);
    g_renderer->EndCameraAndWindow(m_camera, g_window);
}



void Game::Shutdown()
{
    delete m_camera;
    m_camera = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Game::SaveGuestList()
{
    FileWriteFromString(filepath, "Test");
}



//----------------------------------------------------------------------------------------------------------------------
void Game::LoadGuestList()
{
    std::string test;
    FileReadToString(filepath, test);
}
