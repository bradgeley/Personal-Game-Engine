// Bradley Christensen - 2022
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include <string>

#include "Engine/Math/IntVec2.h"


extern class Window* g_window;



//----------------------------------------------------------------------------------------------------------------------
struct WindowConfig
{
    std::string		m_windowTitle = "Untitled App";
    float			m_clientAspect = 2.f;
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
    
    // todo: move to event system
    bool m_isQuitting = false;
    //

private:

    void CreateMainWindow();
    void RunMessagePump();

private:
    
    WindowConfig const  m_config;
    IntVec2             m_dimensiions;
    void*               m_windowHandle   = nullptr; // HWND
    void*               m_displayContext = nullptr; // HDC
};
