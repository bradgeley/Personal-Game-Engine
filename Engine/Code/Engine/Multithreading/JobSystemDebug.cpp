// Bradley Christensen - 2023
#include "JobSystemDebug.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr float WINDOW_WIDTH = 150.f;
constexpr float WINDOW_HEIGHT = 100.f;
constexpr float GRAPH_EDGE_PAD = 5.f;
constexpr float GRAPH_EDGE_THICKNESS = 0.25f;
constexpr float TITLE_FONT_SIZE = 4.f;



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
    m_camera->SetOrthoBounds(Vec3(0.f, 0.f, 0.f), Vec3(WINDOW_WIDTH, WINDOW_HEIGHT, 1.f));
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
    g_renderer->BeginCameraAndWindow(m_camera, g_window);
    g_renderer->ClearScreen(Rgba8::LightGray);

    VertexBuffer buffer;
    AddVertsForWireBox2D(buffer.GetMutableVerts(), AABB2(GRAPH_EDGE_PAD, GRAPH_EDGE_PAD,
        WINDOW_WIDTH - GRAPH_EDGE_PAD, WINDOW_HEIGHT - GRAPH_EDGE_PAD), GRAPH_EDGE_THICKNESS, Rgba8::Black);
    
    g_renderer->DrawVertexBuffer(&buffer);

    VertexBuffer text;
    auto font = g_renderer->GetDefaultFont();
    font->AddVertsForAlignedText2D(text.GetMutableVerts(), Vec2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT - (0.5f * GRAPH_EDGE_PAD)),
        Vec2(0.5f, 0.5f), TITLE_FONT_SIZE, "Job System Debug Graph", Rgba8::White);
    
    font->SetRendererState();
    g_renderer->DrawVertexBuffer(&text);

    g_renderer->Present();
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystemDebug::EndFrame()
{
    m_frameDebugInfo.clear();
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



//----------------------------------------------------------------------------------------------------------------------
void JobSystemDebug::Log(JobDebugInfo& info)
{
    std::unique_lock lock(m_logMutex);
    m_frameDebugInfo.emplace_back(info);
}
