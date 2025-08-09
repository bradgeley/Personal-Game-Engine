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
        m_jobDebugLog.clear();
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
    for (JobDebugInfo const& jobDebug : m_jobDebugLog)
    {
        AddVertsForJob(buffer, jobDebug);
    }
    
    g_renderer->DrawVertexBuffer(&buffer);

    VertexBuffer textBuffer;
    Font* font = g_renderer->GetDefaultFont();
    font->AddVertsForAlignedText2D(textBuffer.GetMutableVerts(), Vec2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT - (0.5f * GRAPH_EDGE_PAD)),
        Vec2(0.f, 0.f), TITLE_FONT_SIZE, "Job System Debug Graph", Rgba8::Black);

    float frameSeconds = m_frameDebugInfo.m_actualDeltaSeconds;
    std::string frameCounterText = StringF("Frame:(%i) FPS(%.2f) Seconds(%.2fms) Draw(%i)", m_frameDebugInfo.m_frameNumber, 1 / frameSeconds, frameSeconds * 1000.f, g_renderer->GetNumFrameDrawCalls());
    font->AddVertsForAlignedText2D(textBuffer.GetMutableVerts(), GetGraphOutline().maxs, Vec2(-1.f, 1.f), TITLE_FONT_SIZE * 0.5f, frameCounterText, Rgba8::Black);

    for (JobDebugInfo const& jobDebug : m_jobDebugLog)
    {
        AddVertsForThreadText(textBuffer, jobDebug);
    }
    
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
void JobSystemDebug::Log(JobDebugInfo const& info)
{
    std::unique_lock lock(m_logMutex);
    if (!m_freezeLog)
    {
        m_jobDebugLog.emplace_back(info);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystemDebug::UpdateFrameDebugInfo(FrameDebugInfo const& info)
{
    if (!m_freezeLog)
    {
        m_frameDebugInfo = info;
    }
}



//----------------------------------------------------------------------------------------------------------------------
int JobSystemDebug::GetFrameNumber() const
{
    return m_frameDebugInfo.m_frameNumber;
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
            for (JobDebugInfo const& line : m_jobDebugLog)
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
    BitArray<128> bitArray;
    for (JobDebugInfo const& debug : m_jobDebugLog)
    {
        ASSERT_OR_DIE(debug.m_threadID <= 64, "Too many threads for preallocated bit array, increase size or reduce thread count.");
        bitArray.Set(debug.m_threadID);
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
void JobSystemDebug::AddVertsForJob(VertexBuffer& vbo, JobDebugInfo const& debugInfo) const
{
    AABB2 jobBounds;
    GetBoundsForJob(jobBounds, debugInfo);
    Rgba8 systemTint = g_ecs->GetSystemByGlobalPriority(debugInfo.m_threadID)->GetDebugTint();
    AddVertsForAABB2(vbo.GetMutableVerts(), jobBounds, systemTint);

    AABB2 threadBounds;
    GetBoundsForThread(threadBounds, debugInfo);
    AddVertsForWireBox2D(vbo.GetMutableVerts(), threadBounds, 0.1f, Rgba8::DarkGray);
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystemDebug::GetBoundsForJob(AABB2& out_jobBounds, JobDebugInfo const& debugInfo) const
{
    AABB2 threadBounds;
    GetBoundsForThread(threadBounds, debugInfo);
    float jobStartFractionX = GetFractionWithin(debugInfo.m_startTime, m_frameDebugInfo.m_ecsFrameStartTime, m_frameDebugInfo.m_ecsFrameStartTime + m_frameDebugInfo.m_actualDeltaSeconds);
    float jobEndFractionX = GetFractionWithin(debugInfo.m_endTime, m_frameDebugInfo.m_ecsFrameStartTime, m_frameDebugInfo.m_ecsFrameStartTime + m_frameDebugInfo.m_actualDeltaSeconds);
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



void JobSystemDebug::GetBoundsForThread(AABB2& out_threadBounds, JobDebugInfo const& debugInfo) const
{
    int numThreads = CountUniqueThreads();
    Vec2 threadMins = GetThreadOrigin(debugInfo.m_threadID, numThreads);
    Vec2 threadMaxs = threadMins + Vec2(GetGraphOutline().GetWidth(), GetGraphOutline().GetHeight() / (float) numThreads);
    out_threadBounds = AABB2(threadMins, threadMaxs);
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystemDebug::AddVertsForThreadText(VertexBuffer& vbo, JobDebugInfo const& debugInfo) const
{
    AABB2 threadBounds;
    GetBoundsForThread(threadBounds, debugInfo);

    Font* font = g_renderer->GetDefaultFont();
    System* system = g_ecs->GetSystemByGlobalPriority(debugInfo.m_threadID);
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