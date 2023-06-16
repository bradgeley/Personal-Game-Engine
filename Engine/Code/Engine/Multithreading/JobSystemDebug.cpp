// Bradley Christensen - 2023
#include "JobSystemDebug.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
// THE JOB SYSTEM DEBUG
//
JobSystemDebug* g_jobSystemDebug = nullptr;



//----------------------------------------------------------------------------------------------------------------------
JobSystemDebug::JobSystemDebug(JobSystemDebugConfig const& config) : m_config(config)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystemDebug::Startup()
{
    WindowConfig windowConfig;
    windowConfig.m_clientAspect = 1.5f;
    windowConfig.m_windowScale = 0.5f;
    windowConfig.m_windowTitle = "Job System Debug Window";
    m_window = new Window(windowConfig);
    m_window->Startup();

    g_renderer->CreateWindowRenderContext(m_window);
    
    m_camera = new Camera();
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystemDebug::BeginFrame()
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystemDebug::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystemDebug::Render() const
{
    g_renderer->BeginWindow(m_window);
    g_renderer->BeginCamera(*m_camera);
    g_renderer->ClearScreen(Rgba8::Blue);

    VertexBuffer buffer;
    AddVertsForRect2D(buffer.GetMutableVerts(), Vec2(0.1f, 0.1f), Vec2(0.2f, 0.2f));
    g_renderer->DrawVertexBuffer(&buffer);

    g_renderer->Present();
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystemDebug::EndFrame()
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystemDebug::Shutdown()
{
    m_window->Shutdown();
    delete m_window;
    m_window = nullptr;
    
    delete m_camera;
    m_camera = nullptr;
}
