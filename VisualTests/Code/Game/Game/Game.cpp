// Bradley Christensen - 2022-2023
#include "Game.h"
#include "GameCommon.h"
#include "WindowsApplication.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Math/MathUtils.h"
#include "Game/Game/NoiseTest.h"
#include "Game/Game/Noise2DTest.h"



//----------------------------------------------------------------------------------------------------------------------
void Game::Startup()
{
    m_testModes.push_back(new NoiseTest());
    m_testModes.push_back(new Noise2DTest());

    m_testModes[m_currentTestMode]->Startup();

    g_devConsole->m_helpDelegate.SubscribeMethod(this, &Game::Help);
}



//----------------------------------------------------------------------------------------------------------------------
void Game::BeginFrame()
{
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
    if (g_input->WasKeyJustReleased(KeyCode::Escape))
    {
        g_app->Quit();
    }

    if (g_input->WasKeyJustPressed(KeyCode::Right))
    {
        m_testModes[m_currentTestMode]->Shutdown();
        m_currentTestMode = IncrementIntInRange(m_currentTestMode, 0, (int) m_testModes.size() - 1, true);
        m_testModes[m_currentTestMode]->Startup();
    }
    else if (g_input->WasKeyJustPressed(KeyCode::Left))
    {
        m_testModes[m_currentTestMode]->Shutdown();
        m_currentTestMode = DecrementIntInRange(m_currentTestMode, 0, (int) m_testModes.size() - 1, true);
        m_testModes[m_currentTestMode]->Startup();
    }
    else
    {
        // Run test frame
        m_testModes[m_currentTestMode]->BeginFrame();
        m_testModes[m_currentTestMode]->Update(deltaSeconds);
        m_testModes[m_currentTestMode]->Render();
        m_testModes[m_currentTestMode]->EndFrame();
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Game::EndFrame()
{
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Render() const
{
}



//----------------------------------------------------------------------------------------------------------------------
void Game::Shutdown()
{
    m_testModes[m_currentTestMode]->Shutdown();
    for (auto& mode : m_testModes)
    {
        delete mode;
    }
    m_testModes.clear();
}



//----------------------------------------------------------------------------------------------------------------------
bool Game::Help(NamedProperties& args)
{
    UNUSED(args)
    g_devConsole->LogWarning("Change test modes: Right/Left arrow");
    m_testModes[m_currentTestMode]->DisplayHelpMessage();
    return true;
}
