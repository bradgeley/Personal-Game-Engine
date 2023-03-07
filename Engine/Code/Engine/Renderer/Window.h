// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Math/IntVec2.h"
#include <string>

#include "Engine/Events/EventDelegate.h"



extern class Window* g_window;



//----------------------------------------------------------------------------------------------------------------------
struct WindowConfig
{
    std::string		m_windowTitle = "Untitled App";
    float			m_clientAspect = 2.f;
    float           m_windowScale = 1.f;
};



//----------------------------------------------------------------------------------------------------------------------
// Window
//
// A Windows window, used for rendering the main game.
//
class Window : public EngineSubsystem
{
public:

    Window(WindowConfig const& config);
    
    virtual void Startup() override;
    virtual void BeginFrame() override;

    void* GetHWND() const;
    int GetWidth() const;
    int GetHeight() const;
    IntVec2 const& GetDimensions() const;

public:

    // Input events
    EventDelegate m_quit;
    EventDelegate m_mouseButtonDownEvent;
    EventDelegate m_mouseButtonUpEvent;
    EventDelegate m_charInputEvent;
    EventDelegate m_keyDownEvent;
    EventDelegate m_keyUpEvent;
    EventDelegate m_mouseWheelEvent;

private:

    void CreateMainWindow();
    void RunMessagePump();

private:
    
    WindowConfig const  m_config;
    IntVec2 m_dimensiions;
    
    void* m_windowHandle   = nullptr; // HWND
    void* m_displayContext = nullptr; // HDC
};
