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
struct WindowConfig
{
    std::string		m_windowTitle = "Untitled Window";
    float			m_clientAspect = 2.f;
    float           m_windowScale = 1.f;
    bool            m_automaticallyPresent = true;
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
    bool IsBeingCreated() const;
    void SetIsBeingCreated(bool isBeingCreated);
    void SetWindowTitle(std::string const& newTitle);

    void* GetHWND() const;
    int GetWidth() const;
    int GetHeight() const;
    float GetAspect() const;
    IntVec2 const& GetDimensions() const;

    IntVec2 GetMouseClientPosition(bool originBottomLeft = true) const;
    Vec2 GetMouseClientRelativePosition(bool originBottomLeft = true) const;

    bool GiveFocus();
    void SetHasFocus(bool hasFocus);

private:

    void CreateMainWindow();
    void RunMessagePump();
    
public:

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

protected:
    
    IntVec2 m_dimensions;

    void* m_windowHandle            = nullptr; // HWND
    void* m_displayContext          = nullptr; // HDC

    bool m_hasFocus                 = false;
    bool m_isBeingCreated           = false;
};
