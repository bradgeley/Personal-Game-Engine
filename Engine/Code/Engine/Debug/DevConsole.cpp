// Bradley Christensen - 2023
#include "Engine/Debug/DevConsole.h"

#include "Engine/Core/EngineCommon.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Input/InputUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
// THE Dev Console
//
DevConsole* g_devConsole = nullptr;



//----------------------------------------------------------------------------------------------------------------------
DevConsole::DevConsole(DevConsoleConfig const& config) : m_config(config)
{
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::Startup()
{
    g_window->m_charInputEvent.SubscribeMethod(this, &DevConsole::HandleChar);
    g_window->m_keyDownEvent.SubscribeMethod(this, &DevConsole::HandleKeyDown);
    g_window->m_keyUpEvent.SubscribeMethod(this, &DevConsole::HandleKeyUp);
    g_window->m_mouseButtonDownEvent.SubscribeMethod(this, &DevConsole::HandleMouseButtonDown);
    g_window->m_mouseButtonUpEvent.SubscribeMethod(this, &DevConsole::HandleMouseButtonUp);
    g_window->m_mouseWheelEvent.SubscribeMethod(this, &DevConsole::HandleMouseWheel);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::Update(float deltaSeconds)
{
    float animSeconds = m_config.m_openCloseAnimationSpeed * deltaSeconds;
    m_openCloseAnimationFraction += (m_isShowing ? -animSeconds : animSeconds);
    m_openCloseAnimationFraction = ClampF(m_openCloseAnimationFraction, 0.f, 1.f);
} 



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::Render() const
{
    if (m_openCloseAnimationFraction == 1.f)
    {
        return;
    }

    float devConsoleOffset = SmoothStart3(m_openCloseAnimationFraction);
    
    g_renderer->BeginCamera(m_camera);

    // Translate by the animation fraction
    Mat44 modelMatrix;
    modelMatrix.Append(Mat44::CreateTranslation(0.f, devConsoleOffset));
    g_renderer->SetModelMatrix(modelMatrix);
    
    VertexBuffer backgroundVbo;
    auto& backgroundVerts = backgroundVbo.GetMutableVerts();
    AddVertsForAABB2(backgroundVerts, m_camera.GetOrthoBounds2D(), m_config.m_backgroundTint);
    g_renderer->DrawVertexBuffer(&backgroundVbo);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{
    g_window->m_charInputEvent.UnsubscribeMethod(this, &DevConsole::HandleChar);
    g_window->m_keyDownEvent.UnsubscribeMethod(this, &DevConsole::HandleKeyDown);
    g_window->m_keyUpEvent.UnsubscribeMethod(this, &DevConsole::HandleKeyUp);
    g_window->m_mouseButtonDownEvent.UnsubscribeMethod(this, &DevConsole::HandleMouseButtonDown);
    g_window->m_mouseButtonUpEvent.UnsubscribeMethod(this, &DevConsole::HandleMouseButtonUp);
    g_window->m_mouseWheelEvent.UnsubscribeMethod(this, &DevConsole::HandleMouseWheel);
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleChar(NamedProperties& args)
{
    int character = args.Get("Char", -1);
    if (character == '`')
    {
        return true;
    }
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleKeyDown(NamedProperties& args)
{
    int key = args.Get("Key", -1);
    if (key == (int) KeyCode::TILDE)
    {
        m_isShowing = !m_isShowing;
        return true;
    }
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleKeyUp(NamedProperties& args)
{
    UNUSED(args)
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleMouseButtonDown(NamedProperties& args)
{
    UNUSED(args)
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleMouseButtonUp(NamedProperties& args)
{
    UNUSED(args)
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleMouseWheel(NamedProperties& args)
{
    UNUSED(args)
    return m_isShowing;
}