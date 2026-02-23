// Bradley Christensen - 2022-2026
#include "Window.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Renderer/Renderer.h"




//----------------------------------------------------------------------------------------------------------------------
// The main game window
//
Window* g_window = nullptr;



//----------------------------------------------------------------------------------------------------------------------
Window::Window(WindowConfig const& config) : EngineSubsystem("Window"), m_config(config), m_userSettings(m_config.m_startupUserSettings)
{
}



//----------------------------------------------------------------------------------------------------------------------
void Window::Startup()
{
    if (this == g_window)
    {
        RegisterEvents();
    }

    MakeWindow();

    if (m_config.m_takeFocusWhenCreated)
    {
        GiveFocus();
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Window::Shutdown()
{
    if (this == g_window)
    {
        UnregisterEvents();
    }

    g_renderer->ReleaseRenderTarget(m_renderTargetID);
}



//----------------------------------------------------------------------------------------------------------------------
int Window::GetWidth() const
{
    IntVec2 resolution = GetActualWindowResolution();
    return resolution.x;
}



//----------------------------------------------------------------------------------------------------------------------
int Window::GetHeight() const
{
    IntVec2 resolution = GetActualWindowResolution();
    return resolution.y;
}



//----------------------------------------------------------------------------------------------------------------------
float Window::GetAspect() const
{
    return (float) GetWidth() / (float) GetHeight();
}



//----------------------------------------------------------------------------------------------------------------------
WindowMode Window::GetCurrentWindowMode() const
{
    return m_userSettings.m_windowMode;
}



//----------------------------------------------------------------------------------------------------------------------
bool Window::IsFullscreen() const
{
    return (m_userSettings.m_windowMode == WindowMode::Fullscreen);
}



//----------------------------------------------------------------------------------------------------------------------
bool Window::IsBorderless() const
{
    return (m_userSettings.m_windowMode == WindowMode::Borderless);
}



//----------------------------------------------------------------------------------------------------------------------
bool Window::IsWindowed() const
{
    return (m_userSettings.m_windowMode == WindowMode::Windowed);
}



//----------------------------------------------------------------------------------------------------------------------
void Window::SetIsMinimized(bool isMinimized)
{
    m_isMinimized = isMinimized;
}



//----------------------------------------------------------------------------------------------------------------------
bool Window::GetIsMinimized() const
{
    return m_isMinimized;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 Window::GetActualWindowResolution() const
{
    return m_actualResolution;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 Window::GetRenderResolution() const
{
    return GetActualWindowResolution() * m_userSettings.m_renderResolutionMultiplier;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 Window::GetLastKnownWindowedPos() const
{
    return m_lastKnownWindowedPos;
}



//----------------------------------------------------------------------------------------------------------------------
void Window::SetHasFocus(bool hasFocus)
{
    m_hasFocus = hasFocus;

    WindowFocusChanged();
}



//----------------------------------------------------------------------------------------------------------------------
bool Window::HasFocus() const
{
    return m_hasFocus;
}



//----------------------------------------------------------------------------------------------------------------------
std::string Window::GetWindowTitle() const
{
    IntVec2 resolution = GetActualWindowResolution();
    IntVec2 renderResolution = GetRenderResolution();
    std::string focusString = m_hasFocus ? "Focused, " : "";
    std::string modeString;
    switch (m_userSettings.m_windowMode)
    {
        case WindowMode::Borderless:
            modeString = "Borderless";
            break;
        case WindowMode::Windowed:
            modeString = "Windowed";
            break;
        case WindowMode::Fullscreen:
            modeString = "Fullscreen";
            break;
    }
    std::string title = StringUtils::StringF("%s (%s%i/%i, %s)", m_config.m_windowTitle.c_str(), focusString.c_str(), resolution.x, resolution.y, modeString.c_str());
    return title;
}



//----------------------------------------------------------------------------------------------------------------------
void Window::RefreshWindowTitle()
{
    std::string windowTitle = GetWindowTitle();
    SetWindowTitle(windowTitle);
}



//----------------------------------------------------------------------------------------------------------------------
void Window::HandleWindowResolutionChanged(int width, int height)
{
    if (width == 0 || height == 0)
    {   
        // Invalid
        return;
    }

    if (m_isManuallyResizing)
    {
        m_userSettings.m_windowedResolution.x = width;
        m_userSettings.m_windowedResolution.y = height;
    }

    m_actualResolution = IntVec2(width, height);

    WindowSizeChanged();
}



//----------------------------------------------------------------------------------------------------------------------
void Window::ToggleUserManualResizing(bool isManuallyResizing)
{
    m_isManuallyResizing = isManuallyResizing;
}



//----------------------------------------------------------------------------------------------------------------------
RenderTargetID Window::GetRenderTarget() const
{
    return m_renderTargetID;
}



//----------------------------------------------------------------------------------------------------------------------
void Window::RegisterEvents()
{
    if (g_eventSystem && g_devConsole)
    {
        g_devConsole->AddDevConsoleCommandInfo("SetWindowMode", "mode", DevConsoleArgType::String);
        g_eventSystem->SubscribeMethod("SetWindowMode", this, &Window::SetWindowMode);
    }
}


//----------------------------------------------------------------------------------------------------------------------
void Window::UnregisterEvents()
{
    if (g_eventSystem)
    {
        g_eventSystem->UnsubscribeMethod("SetWindowMode", this, &Window::SetWindowMode);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Window::WindowSizeChanged()
{
    RefreshWindowTitle();

    if (m_renderTargetID != RendererUtils::InvalidID)
    {
        g_renderer->ResizeSwapChainRenderTarget(m_renderTargetID, GetRenderResolution());
    }

    NamedProperties args;
    args.Set("window", this);
    m_windowSizeChanged.Broadcast(args);
}



//----------------------------------------------------------------------------------------------------------------------
bool Window::SetWindowMode(NamedProperties& args)
{
    WindowMode previousMode = m_userSettings.m_windowMode;

    std::string mode = args.Get("mode", std::string());
    StringUtils::ToLower(mode);
    if (strcmp(mode.c_str(), "borderless") == 0)
    {
        m_userSettings.m_windowMode = WindowMode::Borderless;
    }
    else if (strcmp(mode.c_str(), "windowed") == 0)
    {
        m_userSettings.m_windowMode = WindowMode::Windowed;
    }
    else if (strcmp(mode.c_str(), "fullscreen") == 0)
    {
        m_userSettings.m_windowMode = WindowMode::Fullscreen;
    }

    if (m_userSettings.m_windowMode != previousMode)
    {
        WindowModeChanged(previousMode);
    }
    return false;
}