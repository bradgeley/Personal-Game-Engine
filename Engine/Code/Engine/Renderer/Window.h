// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Events/EventDelegate.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"
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
};



//----------------------------------------------------------------------------------------------------------------------
struct WindowConfig
{
    std::string		m_windowTitle = "Untitled Window";
    float			m_clientAspect = 2.f; // override if m_openInWindowedBorderless = false
    float           m_windowScale = 1.f;  // override if m_openInWindowedBorderless = false
    bool            m_automaticallyPresent = true;

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
    IntVec2 const& GetDimensions() const;
    WindowMode GetCurrentWindowMode() const;
    bool IsFullscreen() const;

    IntVec2 GetMouseClientPosition(bool originBottomLeft = true) const;
    Vec2 GetMouseClientRelativePosition(bool originBottomLeft = true) const;

    bool GiveFocus();
    void SetHasFocus(bool hasFocus);
    bool HasFocus() const;

private:

    void MakeWindow();
    void RunMessagePump();
    void RegisterEvents();
    void UnregisterEvents();

    bool SetWindowMode(NamedProperties& args);
    
public:

    EventDelegate m_focusChanged;
    EventDelegate m_windowModeChanged;

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
    
    IntVec2 m_dimensions;

    void* m_windowHandle            = nullptr; // HWND
    void* m_displayContext          = nullptr; // HDC

    bool m_hasFocus                 = false;
};
