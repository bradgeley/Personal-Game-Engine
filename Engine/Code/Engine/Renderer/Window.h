// Bradley Christensen - 2022
#pragma once
#include "Engine/Core/EngineSubsystem.h"



extern class Window* g_window;



//----------------------------------------------------------------------------------------------------------------------
struct WindowConfig
{
    
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
    
    virtual void Startup();
    virtual void BeginFrame();
    virtual void Update(float deltaSeconds);
    virtual void Render();
    virtual void EndFrame();
    virtual void Shutdown();

private:

    void CreateMainWindow();
    void RunMessagePump();

private:

    WindowConfig const m_config;
};
