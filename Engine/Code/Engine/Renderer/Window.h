// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Events/EventDelegate.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"
#include <string>
#include <vector>



class Texture;
struct RenderTarget;



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
// A Windows window, used for rendering stuff.
//
class Window : public EngineSubsystem
{
public:

    Window(WindowConfig const& config);
    
    virtual void Startup() override;
    virtual void BeginFrame() override;
    virtual void Shutdown() override;

    bool IsValid() const;
    void SetWindowTitle(std::string const& newTitle);

    void* GetHWND() const;
    int GetWidth() const;
    int GetHeight() const;
    float GetAspect() const;

    WindowMode GetCurrentWindowMode() const;
    bool IsFullscreen() const;
    bool IsBorderless() const;
    bool IsWindowed() const;
    void Minimize();
    void SetIsMinimized(bool isMinimized);
    bool GetIsMinimized() const;

    IntVec2 GetDesiredWindowResolution() const;
    IntVec2 GetActualWindowResolution() const;
    IntVec2 GetRenderResolution() const;
    IntVec2 GetLastKnownWindowedPos() const;

    IntVec2 GetMouseClientPosition(bool originBottomLeft = true) const;
    Vec2 GetMouseClientRelativePosition(bool originBottomLeft = true) const;

    bool GiveFocus();
    void SetHasFocus(bool hasFocus);
    bool HasFocus() const;

    uint32_t GetWindowStyleFlags() const;
    uint32_t GetWindowStyleExFlags() const;

    std::string GetWindowTitle() const;
    void RefreshWindowTitle();
    void RefreshWindowSize();
    void HandleWindowResolutionChanged(int width, int height);
    void ToggleUserManualResizing(bool isManuallyResizing);
    void HandleManualMove();

    void MakeChildOf(Window* parentWindow);

    RenderTarget* GetRenderTarget() const;

private:

    void MakeWindow();
    void RunMessagePump();
    void RegisterEvents();
    void UnregisterEvents();

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

    RenderTarget* m_renderTarget    = nullptr;
};
