// Bradley Christensen - 2023
#include "JobSystemDebug.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/DataStructures/BitArray.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr float WINDOW_WIDTH = 150.f;
constexpr float WINDOW_HEIGHT = 100.f;
constexpr float GRAPH_EDGE_PAD = 5.f;
constexpr float GRAPH_EDGE_THICKNESS = 0.25f;
constexpr float TITLE_FONT_SIZE = 4.f;



//----------------------------------------------------------------------------------------------------------------------
// Graph drawing helper functions
static Vec2 GetGraphOrigin();
static AABB2 GetGraphOutline();
static Vec2 GetThreadOrigin(int threadID, int numTotalThreads);
static void AddVertsForJob(VertexBuffer& vbo, JobDebugInfo const& debugInfo);
static void AddVertsForThreadText(VertexBuffer& vbo, JobDebugInfo const& debugInfo);



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

    m_window->m_keyUpEvent.SubscribeMethod(this, &JobSystemDebug::HandleKeyUp);
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystemDebug::BeginFrame()
{
    if (!m_freezeLog)
    {
        m_frameDebugInfo.clear();
    }
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
    g_renderer->BeginCameraAndWindow(m_camera, m_window);
    g_renderer->ClearScreen(Rgba8::LightGray);

    VertexBuffer buffer;
    AddVertsForWireBox2D(buffer.GetMutableVerts(), GetGraphOutline(), GRAPH_EDGE_THICKNESS, Rgba8::Black);
    
    g_renderer->DrawVertexBuffer(&buffer);

    VertexBuffer textBuffer;
    auto font = g_renderer->GetDefaultFont();
    font->AddVertsForAlignedText2D(textBuffer.GetMutableVerts(), Vec2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT - (0.5f * GRAPH_EDGE_PAD)),
        Vec2(0.f, 0.f), TITLE_FONT_SIZE, "Job System Debug Graph", Rgba8::White);
    
    font->SetRendererState();
    g_renderer->DrawVertexBuffer(&textBuffer);
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



//----------------------------------------------------------------------------------------------------------------------
void JobSystemDebug::Log(JobDebugInfo& info)
{
    std::unique_lock lock(m_logMutex);
    if (!m_freezeLog)
    {
        m_frameDebugInfo.emplace_back(info);
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystemDebug::HandleKeyUp(NamedProperties& args)
{
    char character = (char) args.Get("Key", -1);
    if (character == 'F')
    {
        if (m_freezeLog)
        {
            g_devConsole->LogSuccess("Unfroze job system debug.");
        }
        else
        {
            g_devConsole->LogSuccess("Froze job system debug.");
            for (JobDebugInfo const& line : m_frameDebugInfo)
            {
                g_devConsole->LogWarningF("Thread %i: start(%f) end(%f)", line.m_threadID, line.m_startTime, line.m_endTime);
            }
        }
        m_freezeLog = !m_freezeLog;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
int JobSystemDebug::CountUniqueThreads() const
{
    BitArray<64> bitArray;
    for (JobDebugInfo const& debug : m_frameDebugInfo)
    {
        ASSERT_OR_DIE(debug.m_threadID <= 64, "Too many threads for preallocated bit array, increase size or reduce thread count.");
        bitArray.Set(debug.m_threadID);
    }
    return bitArray.CountSetBits();
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 GetGraphOrigin()
{
    return GetGraphOutline().mins; 
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 GetGraphOutline()
{
    static AABB2 outline = AABB2(GRAPH_EDGE_PAD, GRAPH_EDGE_PAD, WINDOW_WIDTH - GRAPH_EDGE_PAD, WINDOW_HEIGHT - GRAPH_EDGE_PAD);
    return outline;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 GetThreadOrigin(int threadID, int numTotalThreads)
{
    float thickness = GetGraphOutline().GetHeight() / (float) numTotalThreads;
    return GetGraphOrigin() + Vec2(0.f, (float) threadID * thickness);
}



// Graph is constructed as follows:

// |--------------------------------------------------------------------------------------|
// | Thread N |                       |    |                                              |
// | ...      |                  |    |                                                   |
// | Thread 0 | |               |                                                         |
// |--------------------------------------------------------------------------------------|
//             ^ 0 ms                                            ~deltaSeconds ^ (padding)

//----------------------------------------------------------------------------------------------------------------------
void AddVertsForJob(VertexBuffer& vbo, JobDebugInfo const& debugInfo)
{ 

}



//----------------------------------------------------------------------------------------------------------------------
void AddVertsForThreadText(VertexBuffer& vbo, JobDebugInfo const& debugInfo)
{

}