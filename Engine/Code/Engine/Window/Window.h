// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Events/EventDelegate.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Renderer/RendererUtils.h"
#include <string>
#include <vector>



class Texture;



//----------------------------------------------------------------------------------------------------------------------
// The main game window
//
extern class Window* g_window;



//----------------------------------------------------------------------------------------------------------------------
enum class WindowMode : uint8_t
{
    Windowed,
    Fullscreen,
    Borderless,
};



//----------------------------------------------------------------------------------------------------------------------
struct WindowUserSettings
{
    WindowMode m_windowMode = WindowMode::Borderless;
    IntVec2 m_windowedResolution = IntVec2(1920, 1080); // in fullscreen and borderless, this will forced to be the desktop size
    float m_renderResolutionMultiplier = 1.f; // should be between 0 and 1, but close to 0 would cause issues.
};



//----------------------------------------------------------------------------------------------------------------------
struct WindowConfig
{
    std::string		m_windowTitle = "Untitled Window";
    bool            m_automaticallyPresent = true;
    bool            m_takeFocusWhenCreated = true;

    WindowUserSettings m_startupUserSettings;
};



//----------------------------------------------------------------------------------------------------------------------
// Window
//
// A window, used for rendering stuff.
//
class Window : public EngineSubsystem
{
public:

    Window(WindowConfig const& config);
    
    virtual void Startup() override;
    virtual void Shutdown() override;

    virtual bool IsValid() const = 0;
    virtual void SetWindowTitle(std::string const& newTitle) = 0;

    int GetWidth() const;
    int GetHeight() const;
    float GetAspect() const;

    WindowMode GetCurrentWindowMode() const;
    bool IsFullscreen() const;
    bool IsBorderless() const;
    bool IsWindowed() const;
    virtual void Minimize() = 0;
    void SetIsMinimized(bool isMinimized);
    bool GetIsMinimized() const;

    virtual IntVec2 GetDesiredWindowResolution() const = 0;
    IntVec2 GetActualWindowResolution() const;
    IntVec2 GetRenderResolution() const;
    IntVec2 GetLastKnownWindowedPos() const;

    virtual IntVec2 GetMouseClientPosition(bool originBottomLeft = true) const = 0;
    virtual Vec2 GetMouseClientRelativePosition(bool originBottomLeft = true) const = 0;

    virtual bool GiveFocus() = 0;
    void SetHasFocus(bool hasFocus);
    bool HasFocus() const;

    std::string GetWindowTitle() const;
    void RefreshWindowTitle();
    virtual void RefreshWindowSize() = 0;
    void HandleWindowResolutionChanged(int width, int height);
    void ToggleUserManualResizing(bool isManuallyResizing);
    virtual void HandleManualMove() = 0;

    RenderTargetID GetRenderTarget() const;

private:

    virtual void MakeWindow() = 0;
    void RegisterEvents();
    void UnregisterEvents();

    virtual void WindowModeChanged(WindowMode previousMode) = 0;
    virtual void WindowFocusChanged() = 0;
    void WindowSizeChanged();

    bool SetWindowMode(NamedProperties& args);
    
public:

    EventDelegate m_focusChanged;
    EventDelegate m_windowModeChanged;
    EventDelegate m_windowSizeChanged;

    // The input system listens to these events from the window and turns them into input events
    EventDelegate m_quit;
    EventDelegate m_mouseButtonDownEvent;
    EventDelegate m_mouseButtonUpEvent;
    EventDelegate m_charInputEvent;
    EventDelegate m_keyDownEvent;
    EventDelegate m_keyUpEvent;
    EventDelegate m_mouseWheelEvent;

public:
    
    WindowConfig const m_config;
    WindowUserSettings m_userSettings;

protected:

    IntVec2 m_actualResolution      = IntVec2::ZeroVector;
    IntVec2 m_lastKnownWindowedPos  = IntVec2(100, 100);

    void* m_windowHandle            = nullptr; // HWND
    void* m_displayContext          = nullptr; // HDC

    bool m_hasFocus                 = false;
    bool m_isManuallyResizing       = false;
    bool m_isMinimized              = false;

    RenderTargetID m_renderTargetID = RendererUtils::InvalidID;
};
