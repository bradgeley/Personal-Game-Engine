// Bradley Christensen - 2023
#include "PerformanceDebugWindow.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/DataStructures/BitArray.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr float WINDOW_WIDTH = 150.f;
constexpr float WINDOW_HEIGHT = 100.f;
constexpr float GRAPH_EDGE_PAD = 5.f;
constexpr float GRAPH_LEFT_EDGE_PAD = 25.f;
constexpr float GRAPH_EDGE_THICKNESS = 0.25f;
constexpr float TITLE_FONT_SIZE = 4.f;
constexpr float JOB_MIN_THICKNESS = 0.5f;



//----------------------------------------------------------------------------------------------------------------------
// Graph drawing helper functions
static Vec2 GetGraphOrigin();
static AABB2 GetGraphOutline();
static Vec2 GetThreadOrigin(int threadID, int numTotalThreads);



//----------------------------------------------------------------------------------------------------------------------
// THE JOB SYSTEM DEBUG
//
PerformanceDebugWindow* g_performanceDebugWindow = nullptr;



//----------------------------------------------------------------------------------------------------------------------
PerformanceDebugWindow::PerformanceDebugWindow(PerformanceDebugWindowConfig const& config) : m_config(config)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::Startup()
{
    WindowConfig windowConfig;
    windowConfig.m_clientAspect = 1.5f;
    windowConfig.m_windowScale = 0.5f;
    windowConfig.m_windowTitle = "Performance Debug Window";
    m_window = new Window(windowConfig);
    m_window->Startup();

    g_renderer->GetOrCreateWindowRenderContext(m_window);
    
    m_camera = new Camera();
    m_camera->SetOrthoBounds(Vec3(0.f, 0.f, 0.f), Vec3(WINDOW_WIDTH, WINDOW_HEIGHT, 1.f));

    m_window->m_keyUpEvent.SubscribeMethod(this, &PerformanceDebugWindow::HandleKeyUp);
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::BeginFrame()
{
    std::unique_lock lock(m_perfWindowMutex);
    if (!m_freezeLog)
    {
        m_perfItemData.clear();
    }
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::Render() const
{
    g_renderer->BeginCameraAndWindow(m_camera, m_window);
    g_renderer->ClearScreen(Rgba8::LightGray);

    VertexBuffer buffer;
    AddVertsForWireBox2D(buffer.GetMutableVerts(), GetGraphOutline(), GRAPH_EDGE_THICKNESS, Rgba8::Black);
    for (PerfItemData const& perfData : m_perfItemData)
    {
        AddVertsForData(buffer, perfData);
    }
    
    g_renderer->DrawVertexBuffer(&buffer);

    VertexBuffer textBuffer;
    Font* font = g_renderer->GetDefaultFont();
    font->AddVertsForAlignedText2D(textBuffer.GetMutableVerts(), Vec2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT - (0.5f * GRAPH_EDGE_PAD)),
        Vec2(0.f, 0.f), TITLE_FONT_SIZE, "Job System Debug Graph", Rgba8::Black);

    float frameSeconds = m_perfFrameData.m_actualDeltaSeconds;
    std::string frameCounterText = StringF("Frame:(%i) FPS(%.2f) Seconds(%.2fms) Draw(%i)", m_perfFrameData.m_frameNumber, 1 / frameSeconds, frameSeconds * 1000.f, g_renderer->GetNumFrameDrawCalls());
    font->AddVertsForAlignedText2D(textBuffer.GetMutableVerts(), GetGraphOutline().maxs, Vec2(-1.f, 1.f), TITLE_FONT_SIZE * 0.5f, frameCounterText, Rgba8::Black);

    for (PerfItemData const& perfData : m_perfItemData)
    {
        AddVertsForRowText(textBuffer, perfData);
    }
    
    font->SetRendererState();
    g_renderer->DrawVertexBuffer(&textBuffer);
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::EndFrame()
{
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::Shutdown()
{
    m_window->Shutdown();
    delete m_window;
    m_window = nullptr;
    
    delete m_camera;
    m_camera = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::LogData(PerfItemData const& info)
{
    std::unique_lock lock(m_perfWindowMutex);
    if (!m_freezeLog)
    {
        m_perfItemData.emplace_back(info);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::UpdatePerfWindowFrameData(PerfFrameData const& info)
{
    if (!m_freezeLog)
    {
        m_perfFrameData = info;
    }
}



//----------------------------------------------------------------------------------------------------------------------
int PerformanceDebugWindow::GetFrameNumber() const
{
    return m_perfFrameData.m_frameNumber;
}



//----------------------------------------------------------------------------------------------------------------------
bool PerformanceDebugWindow::HandleKeyUp(NamedProperties& args)
{
    char character = (char) args.Get("Key", -1);
    if (character == 'F')
    {
        if (m_freezeLog)
        {
            g_devConsole->LogSuccess("Unfroze performance debug window.");
        }
        else
        {
            g_devConsole->LogSuccess("Froze performance debug window.");
        }
        m_freezeLog = !m_freezeLog;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
int PerformanceDebugWindow::CountNumRows() const
{
    constexpr int bitArraySize = 128;
    BitArray<bitArraySize> bitArray;
    for (PerfItemData const& debug : m_perfItemData)
    {
        ASSERT_OR_DIE(debug.m_perfRowIndex <= bitArraySize, "Too many threads for preallocated bit array, increase size or reduce thread count.");
        bitArray.Set(debug.m_perfRowIndex);
    }
    return bitArray.CountSetBits();
}



//----------------------------------------------------------------------------------------------------------------------
// Graph is constructed as follows:
//
// |--------------------------------------------------------------------------------------|
// | Thread N |                       |    |                                              |
// | ...      |                  |    |                                                   |
// | Thread 0 | |               |                                                         |
// |--------------------------------------------------------------------------------------|
//             ^ 0 ms                                            ~deltaSeconds ^ (padding)
//

//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::AddVertsForData(VertexBuffer& vbo, PerfItemData const& debugInfo) const
{
    AABB2 jobBounds;
    GetBoundsForData(jobBounds, debugInfo);
    Rgba8 systemTint = g_ecs->GetSystemByGlobalPriority(debugInfo.m_perfRowIndex)->GetDebugTint();
    AddVertsForAABB2(vbo.GetMutableVerts(), jobBounds, systemTint);

    AABB2 threadBounds;
    GetBoundsForRow(threadBounds, debugInfo);
    AddVertsForWireBox2D(vbo.GetMutableVerts(), threadBounds, 0.1f, Rgba8::DarkGray);
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::GetBoundsForData(AABB2& out_jobBounds, PerfItemData const& debugInfo) const
{
    AABB2 threadBounds;
    GetBoundsForRow(threadBounds, debugInfo);
    float jobStartFractionX = GetFractionWithin(debugInfo.m_startTime, m_perfFrameData.m_engineFrameStartTime, m_perfFrameData.m_engineFrameEndTime);
    float jobEndFractionX = GetFractionWithin(debugInfo.m_endTime, m_perfFrameData.m_engineFrameStartTime, m_perfFrameData.m_engineFrameEndTime);
    float jobStartX = jobStartFractionX * threadBounds.GetWidth();
    float jobEndX = jobEndFractionX * threadBounds.GetWidth();
    Vec2 jobMins = Vec2(threadBounds.mins.x + jobStartX, threadBounds.mins.y);
    Vec2 jobMaxs = Vec2(threadBounds.mins.x + jobEndX, threadBounds.maxs.y);
    if (jobMaxs.x - jobMins.x < JOB_MIN_THICKNESS)
    {
        jobMaxs.x = jobMins.x + JOB_MIN_THICKNESS;
    }
    out_jobBounds = AABB2(jobMins, jobMaxs);
}



void PerformanceDebugWindow::GetBoundsForRow(AABB2& out_rowBounds, PerfItemData const& debugInfo) const
{
    int numRows = CountNumRows();
    Vec2 threadMins = GetThreadOrigin(debugInfo.m_perfRowIndex, numRows);
    Vec2 threadMaxs = threadMins + Vec2(GetGraphOutline().GetWidth(), GetGraphOutline().GetHeight() / (float) numRows);
    out_rowBounds = AABB2(threadMins, threadMaxs);
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::AddVertsForRowText(VertexBuffer& vbo, PerfItemData const& debugInfo) const
{
    AABB2 threadBounds;
    GetBoundsForRow(threadBounds, debugInfo);

    Font* font = g_renderer->GetDefaultFont();
    System* system = g_ecs->GetSystemByGlobalPriority(debugInfo.m_perfRowIndex);
    font->AddVertsForAlignedText2D(vbo.GetMutableVerts(), Vec2(threadBounds.GetCenterLeft().x - 2.5, threadBounds.GetCenterLeft().y), Vec2(-1, 0), threadBounds.GetHeight() * 0.5f, system->GetName());
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 GetGraphOrigin()
{
    return GetGraphOutline().mins; 
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 GetGraphOutline()
{
    static AABB2 outline = AABB2(GRAPH_LEFT_EDGE_PAD, GRAPH_EDGE_PAD, WINDOW_WIDTH - GRAPH_EDGE_PAD, WINDOW_HEIGHT - GRAPH_EDGE_PAD);
    return outline;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 GetThreadOrigin(int threadID, int numTotalThreads)
{
    float thickness = GetGraphOutline().GetHeight() / (float) numTotalThreads;
    return GetGraphOrigin() + Vec2(0.f, (float) threadID * thickness);
}