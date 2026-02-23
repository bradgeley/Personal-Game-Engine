// Bradley Christensen - 2022-2026
#include "PerformanceDebugWindow.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/DataStructures/BitArray.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/ECS/System.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Window/Window.h"
#include "Engine/Window/WindowUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Performance/ScopedTimer.h"
#include <cfloat>



//----------------------------------------------------------------------------------------------------------------------
constexpr int WINDOW_RESOLUTION_X = 750;
constexpr int WINDOW_RESOLUTION_Y = 750;
constexpr float GRAPH_EDGE_PAD = 25.f;
constexpr float GRAPH_LEFT_EDGE_PAD = 75.f;
constexpr float GRAPH_OUTLINE_THICKNESS = 1.f;
constexpr float GRAPH_SECTION_OUTLINE_THICKNESS = 1.f;
constexpr float GRAPH_ROW_OUTLINE_THICKNESS = 0.25f;
constexpr float TITLE_FONT_SIZE = 25.f;
constexpr float FPS_COUNTER_FONT_SIZE = 0.75f * TITLE_FONT_SIZE;
constexpr float SECTION_NAME_FONT_SIZE = 25.f;
constexpr float SECTION_NAME_PADDING = 5.f;
constexpr float ROW_NAME_PADDING = 5.f;



//----------------------------------------------------------------------------------------------------------------------
// THE Performance Debug Window
//
PerformanceDebugWindow* g_performanceDebugWindow = nullptr;



//----------------------------------------------------------------------------------------------------------------------
PerformanceDebugWindow::PerformanceDebugWindow(PerformanceDebugWindowConfig const& config) : EngineSubsystem("Performance Debug Window"), m_config(config)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::Startup()
{
    std::unique_lock lock(m_mutex);

    m_untexturedVBO = g_renderer->MakeVertexBuffer<Vertex_PCU>();
    m_textVBO = g_renderer->MakeVertexBuffer<Vertex_PCU>();

    if (m_config.m_startOpen)
    {
        OpenWindow();
    }

    g_input->m_keyUpEvent.SubscribeMethod(this, &PerformanceDebugWindow::HandleMainWindowKeyUp);
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::BeginFrame()
{
    std::unique_lock lock(m_mutex);
    if (!m_freezeLog)
    {
        for (PerfSection& ps : m_perfSections)
        {
            for (PerfRow& row : ps.m_perfRows)
            {
                row.m_perfItemData.clear();
            }
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::Update(float)
{

}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::Render() const
{
    // empty, this window needs to wait until all main window rendering is completed
    // Rendering happens on EngineFrameCompleted
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::EndFrame()
{

}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::Shutdown()
{
    std::unique_lock lock(m_mutex);

    if (g_input)
    {
        g_input->m_keyUpEvent.UnsubscribeMethod(this, &PerformanceDebugWindow::HandleMainWindowKeyUp);
    }

    g_renderer->ReleaseVertexBuffer(m_textVBO);
    g_renderer->ReleaseVertexBuffer(m_untexturedVBO);

    for (PerfSection& ps : m_perfSections)
    {
        for (PerfRow& row : ps.m_perfRows)
        {
            row.m_perfItemData.clear();
        }
        ps.m_perfRows.clear();
    }
    m_perfSections.clear();

    if (m_window)
    {
        m_window->Shutdown();
        delete m_window;
        m_window = nullptr;
    }
    
    if (m_camera)
    {
        delete m_camera;
        m_camera = nullptr;
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool PerformanceDebugWindow::OpenWindow()
{
    if (m_window)
    {
        return false;
    }

    WindowConfig windowConfig;
    windowConfig.m_windowTitle = "Performance Debug Window";
    windowConfig.m_automaticallyPresent = false;
    windowConfig.m_startupUserSettings.m_windowMode = WindowMode::Windowed;
    windowConfig.m_startupUserSettings.m_windowedResolution = IntVec2(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y);
    windowConfig.m_startupUserSettings.m_renderResolutionMultiplier = 1.f;

	m_window = WindowUtils::MakeWindow(windowConfig);
    m_window->Startup();

    IntVec2 renderResolution = m_window->GetRenderResolution();
    AABB2 bounds = AABB2(Vec2(IntVec2::ZeroVector), Vec2(renderResolution));

    m_camera = new Camera();
    m_camera->SetOrthoBounds2D(bounds);

    m_window->m_keyUpEvent.SubscribeMethod(this, &PerformanceDebugWindow::HandlePerfWindowKeyUp);
    m_window->m_quit.SubscribeMethod(this, &PerformanceDebugWindow::HandleWindowQuit);
    m_window->m_windowSizeChanged.SubscribeMethod(this, &PerformanceDebugWindow::WindowSizeChanged);

    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool PerformanceDebugWindow::CloseWindow()
{
    if (m_window)
    {
        m_window->m_keyUpEvent.UnsubscribeMethod(this, &PerformanceDebugWindow::HandlePerfWindowKeyUp);
        m_window->m_quit.UnsubscribeMethod(this, &PerformanceDebugWindow::HandleWindowQuit);
        m_window->m_windowSizeChanged.UnsubscribeMethod(this, &PerformanceDebugWindow::WindowSizeChanged);

        m_window->Shutdown();
        delete m_window;
        m_window = nullptr;
    }

    if (m_camera)
    {
        delete m_camera;
        m_camera = nullptr;
    }

    return false;
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::LogItem(PerfItemData const& info, int sectionID, int rowID)
{
    std::unique_lock lock(m_mutex);
    if (!m_freezeLog)
    {
        PerfSection* section = FindPerfSection(sectionID);
        ASSERT_OR_DIE(section != nullptr, "Cannot find section to log perf data.")
        if (section)
        {
            ASSERT_OR_DIE(rowID < (int) section->m_perfRows.size(), "Invalid row ID for section");
            section->m_perfRows[rowID].m_perfItemData.push_back(info);
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::LogItem(PerfItemData const& item, Name sectionName, Name rowName)
{
    std::unique_lock lock(m_mutex);
    if (!m_freezeLog)
    {
        PerfSection& section = GetOrCreatePerfSection(sectionName);
        PerfRow& row = GetOrCreatePerfRow(section, rowName);
        row.m_perfItemData.push_back(item);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::UpdateFrameData(PerfFrameData const& info)
{
    std::unique_lock lock(m_mutex);
    if (!m_freezeLog)
    {
        m_perfFrameData = info;
    }
}



//----------------------------------------------------------------------------------------------------------------------
int PerformanceDebugWindow::GetFrameNumber()
{
    std::unique_lock lock(m_mutex);
    return m_perfFrameData.m_frameNumber;
}



//----------------------------------------------------------------------------------------------------------------------
int PerformanceDebugWindow::GetOrCreateSectionID(Name sectionName)
{
    std::unique_lock lock(m_mutex);
    PerfSection* existingSection = FindPerfSection(sectionName);
    if (existingSection)
    {
        return existingSection->m_id;
    }

    return CreatePerfSection(sectionName);
}



//----------------------------------------------------------------------------------------------------------------------
int PerformanceDebugWindow::GetOrCreateRowID(int sectionID, Name rowName)
{
    std::unique_lock lock(m_mutex);
    PerfSection* section = FindPerfSection(sectionID);
    if (!section)
    {
        return -1;
    }

    PerfRow& perfRow = GetOrCreatePerfRow(*section, rowName);
    return perfRow.m_id;
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::EngineFrameCompleted()
{
    std::unique_lock lock(m_mutex);
    if (!m_window)
    {
        return;
    }

    g_renderer->BeginCameraAndWindow(m_camera, m_window);
    g_renderer->ClearScreen(Rgba8::LightGray);

    VertexBuffer& textVBO = *g_renderer->GetVertexBuffer(m_textVBO);
    VertexBuffer& untexturedVBO = *g_renderer->GetVertexBuffer(m_untexturedVBO);
    textVBO.ClearVerts();
    untexturedVBO.ClearVerts();

    for (PerfSection const& section : m_perfSections)
    {
        AddUntexturedVertsForSection(untexturedVBO, section);
    }

    // Graph Outline Box
    AABB2 graphOutline;
    GetGraphOutline(graphOutline);
    VertexUtils::AddVertsForWireBox2D(untexturedVBO, graphOutline, GRAPH_OUTLINE_THICKNESS, m_config.m_graphOutlineTint);

    g_renderer->DrawVertexBuffer(untexturedVBO);

    // Title
    Font* font = g_renderer->GetDefaultFont();
    font->AddVertsForAlignedText2D(textVBO, graphOutline.GetTopLeft(), Vec2(1.f, 1.f), TITLE_FONT_SIZE, "Job System Debug Graph", Rgba8::Black);

    // FPS Counter
    float frameSeconds = static_cast<float>(m_perfFrameData.m_actualDeltaSeconds);
    std::string frameCounterText = StringUtils::StringF("Frame:(%i) FPS(%.2f) Time(%.2fms) Draw(%i)", m_perfFrameData.m_frameNumber, 1 / frameSeconds, frameSeconds * 1000.f, g_renderer->GetNumFrameDrawCalls());
    font->AddVertsForAlignedText2D(textVBO, graphOutline.maxs, Vec2(-1.f, 1.f), FPS_COUNTER_FONT_SIZE, frameCounterText, Rgba8::Black);

    // X-Axis Frame Time
    Vec2 frameBounds = GetItemFrameBounds();
    font->AddVertsForAlignedText2D(textVBO, graphOutline.mins, Vec2(1.f, -1.f), FPS_COUNTER_FONT_SIZE, "0", Rgba8::Black);
    font->AddVertsForAlignedText2D(textVBO, graphOutline.GetBottomRight(), Vec2(-1.f, -1.f), FPS_COUNTER_FONT_SIZE, StringUtils::StringF("%0.3fms", (frameBounds.y - frameBounds.x) * 1000.f), Rgba8::Black);

    for (PerfSection const& section : m_perfSections)
    {
        AddTextVertsForSection(textVBO, section);
    }

    font->SetRendererState();
    g_renderer->DrawVertexBuffer(m_textVBO);

    // Custom present timing, so that the main game window finishes rendering before we display on our secondary window
    g_renderer->Present();
}



//----------------------------------------------------------------------------------------------------------------------
bool PerformanceDebugWindow::HandleMainWindowKeyUp(NamedProperties& args)
{
    std::unique_lock lock(m_mutex);
    unsigned char character = (unsigned char) args.Get("Key", -1);
    if (character == 'F')
    {
        if (m_freezeLog)
        {
			DevConsoleUtils::LogSuccess("Unfroze performance debug window.");
        }
        else
        {
            DevConsoleUtils::LogSuccess("Froze performance debug window.");
        }
        m_freezeLog = !m_freezeLog;
    }
    else if (character == (unsigned char) KeyCode::F1)
    {
        if (!OpenWindow())
        {
            m_window->GiveFocus();
        }
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool PerformanceDebugWindow::HandlePerfWindowKeyUp(NamedProperties& args)
{
    std::unique_lock lock(m_mutex);
    unsigned char character = (unsigned char) args.Get("Key", -1);
    if (character == 'F')
    {
        if (m_freezeLog)
        {
            DevConsoleUtils::LogSuccess("Unfroze performance debug window.");
        }
        else
        {
            DevConsoleUtils::LogSuccess("Froze performance debug window.");
        }
        m_freezeLog = !m_freezeLog;
    }
    else if (character == (unsigned char) KeyCode::Escape)
    {
        CloseWindow();
    }

    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool PerformanceDebugWindow::WindowSizeChanged(NamedProperties&)
{
    IntVec2 renderResolution = m_window->GetRenderResolution();
    AABB2 bounds = AABB2(Vec2(IntVec2::ZeroVector), Vec2(renderResolution));
    m_camera->SetOrthoBounds2D(bounds);
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool PerformanceDebugWindow::HandleWindowQuit(NamedProperties&)
{
    CloseWindow();

    return false;
}



//----------------------------------------------------------------------------------------------------------------------
int PerformanceDebugWindow::CountNumRows() const
{
    int numRows = 0;
    for (PerfSection const& section : m_perfSections)
    {
        numRows += (int) section.m_perfRows.size();
    }
    return numRows;
}



//----------------------------------------------------------------------------------------------------------------------
int PerformanceDebugWindow::CountNumRowsBeforeSection(int sectionID) const
{
    ASSERT_OR_DIE(sectionID < (int) m_perfSections.size(), "Invalid row ID for section");

    int numRows = 0;
    for (int i = 0; i < sectionID; ++i)
    {
        if (i >= sectionID)
        {
            break;
        }
        numRows += (int) m_perfSections[i].m_perfRows.size();
    }
    return numRows;
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::AddUntexturedVertsForSection(VertexBuffer& untexturedVerts, PerfSection const& section)
{
    // Section outline
    AABB2 graphOutline;
    GetGraphOutline(graphOutline);
    int numTotalRows = CountNumRows();
    int numRowsBefore = CountNumRowsBeforeSection(section.m_id);
    float sectionHeightFraction = static_cast<float>(section.m_perfRows.size()) / static_cast<float>(numTotalRows);
    float sectionMinsYFraction = static_cast<float>(numRowsBefore) / static_cast<float>(numTotalRows);
    AABB2 sectionOutline;
    sectionOutline.mins = Vec2(graphOutline.mins.x, graphOutline.mins.y + graphOutline.GetHeight() * sectionMinsYFraction);
    sectionOutline.maxs = Vec2(graphOutline.maxs.x, sectionOutline.mins.y + graphOutline.GetHeight() * sectionHeightFraction);
    VertexUtils::AddVertsForWireBox2D(untexturedVerts, sectionOutline, GRAPH_SECTION_OUTLINE_THICKNESS, Rgba8::Black);

    for (PerfRow const& row : section.m_perfRows)
    {
        AddUntexturedVertsForRow(untexturedVerts, section, row);
    }
}





//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::AddTextVertsForSection(VertexBuffer& textVerts, PerfSection const& section)
{
    UNUSED(textVerts);
    UNUSED(section);

    int numTotalRows = CountNumRows();
    int numRowsBefore = CountNumRowsBeforeSection(section.m_id);
    AABB2 graphOutline;
    GetGraphOutline(graphOutline);
    float sectionHeightFraction = static_cast<float>(section.m_perfRows.size()) / static_cast<float>(numTotalRows);
    float sectionMinsYFraction = static_cast<float>(numRowsBefore) / static_cast<float>(numTotalRows);
    AABB2 sectionOutline;
    sectionOutline.mins = Vec2(graphOutline.mins.x, graphOutline.mins.y + graphOutline.GetHeight() * sectionMinsYFraction);
    sectionOutline.maxs = Vec2(graphOutline.maxs.x, sectionOutline.mins.y + graphOutline.GetHeight() * sectionHeightFraction);

    g_renderer->GetDefaultFont()->AddVertsForAlignedText2D(textVerts, sectionOutline.GetCenterLeft() - Vec2(SECTION_NAME_PADDING, 0.f), Vec2(-1, 0), SECTION_NAME_FONT_SIZE, section.m_name.ToString());

    for (PerfRow const& row : section.m_perfRows)
    {
        AddTextVertsForRow(textVerts, section, row);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::AddUntexturedVertsForRow(VertexBuffer& untexturedVerts, PerfSection const& section, PerfRow const& row)
{
    // Row outline
    int numTotalRows = CountNumRows();
    int numRowsBefore = CountNumRowsBeforeSection(section.m_id);
    AABB2 graphOutline;
    GetGraphOutline(graphOutline);

    float sectionHeightFraction = static_cast<float>(section.m_perfRows.size()) / static_cast<float>(numTotalRows);
    float sectionMinsYFraction = static_cast<float>(numRowsBefore) / static_cast<float>(numTotalRows);

    AABB2 sectionOutline;
    sectionOutline.mins = Vec2(graphOutline.mins.x, graphOutline.mins.y + graphOutline.GetHeight() * sectionMinsYFraction);
    sectionOutline.maxs = Vec2(graphOutline.maxs.x, sectionOutline.mins.y + graphOutline.GetHeight() * sectionHeightFraction);

    float rowMinsYFraction = static_cast<float>(row.m_id) / static_cast<float>(section.m_perfRows.size());
    float rowHeight = 1.f / numTotalRows * graphOutline.GetHeight();

    AABB2 rowOutline;
    rowOutline.mins = Vec2(sectionOutline.mins.x, sectionOutline.mins.y + sectionOutline.GetHeight() * rowMinsYFraction);
    rowOutline.maxs = Vec2(graphOutline.maxs.x, rowOutline.mins.y + rowHeight);

    Vec2 allItemTimeBounds = GetItemFrameBounds();

    // Add items
    for (PerfItemData const& item : row.m_perfItemData)
    {
        float itemStartTimeFraction = (float) MathUtils::GetFractionWithin(item.m_startTime, allItemTimeBounds.x, allItemTimeBounds.y);
        float itemEndTimeFraction = (float) MathUtils::GetFractionWithin(item.m_endTime, allItemTimeBounds.x, allItemTimeBounds.y);
        AABB2 itemOutline;
        itemOutline.mins.x = rowOutline.mins.x + rowOutline.GetWidth() * itemStartTimeFraction;
        itemOutline.mins.y = rowOutline.mins.y;
        itemOutline.maxs.x = rowOutline.mins.x + rowOutline.GetWidth() * itemEndTimeFraction;
        itemOutline.maxs.y = rowOutline.maxs.y;

        VertexUtils::AddVertsForAABB2(untexturedVerts, itemOutline, item.m_tint);
    }

    VertexUtils::AddVertsForWireBox2D(untexturedVerts, rowOutline, GRAPH_ROW_OUTLINE_THICKNESS, Rgba8::Black);
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::AddTextVertsForRow(VertexBuffer& textVerts, PerfSection const& section, PerfRow const& row)
{
    // Row outline
    int numTotalRows = CountNumRows();
    int numRowsBefore = CountNumRowsBeforeSection(section.m_id);
    AABB2 graphOutline;
    GetGraphOutline(graphOutline);

    float sectionHeightFraction = static_cast<float>(section.m_perfRows.size()) / static_cast<float>(numTotalRows);
    float sectionMinsYFraction = static_cast<float>(numRowsBefore) / static_cast<float>(numTotalRows);

    AABB2 sectionOutline;
    sectionOutline.mins = Vec2(graphOutline.mins.x, graphOutline.mins.y + graphOutline.GetHeight() * sectionMinsYFraction);
    sectionOutline.maxs = Vec2(graphOutline.maxs.x, sectionOutline.mins.y + graphOutline.GetHeight() * sectionHeightFraction);

    float rowMinsYFraction = static_cast<float>(row.m_id) / static_cast<float>(section.m_perfRows.size());
    float rowHeight = 1.f / numTotalRows * graphOutline.GetHeight();

    AABB2 rowOutline;
    rowOutline.mins = Vec2(sectionOutline.mins.x, sectionOutline.mins.y + sectionOutline.GetHeight() * rowMinsYFraction);
    rowOutline.maxs = Vec2(graphOutline.maxs.x, rowOutline.mins.y + rowHeight);

    Vec2 allItemTimeBounds = GetItemFrameBounds();

    // Add items
    for (PerfItemData const& item : row.m_perfItemData)
    {
        float itemStartTimeFraction = (float) MathUtils::GetFractionWithin(item.m_startTime, allItemTimeBounds.x, allItemTimeBounds.y);
        float itemEndTimeFraction = (float) MathUtils::GetFractionWithin(item.m_endTime, allItemTimeBounds.x, allItemTimeBounds.y);
        AABB2 itemOutline;
        itemOutline.mins.x = rowOutline.mins.x + rowOutline.GetWidth() * itemStartTimeFraction;
        itemOutline.mins.y = rowOutline.mins.y;
        itemOutline.maxs.x = rowOutline.mins.x + rowOutline.GetWidth() * itemEndTimeFraction;
        itemOutline.maxs.y = rowOutline.maxs.y;
    }

    g_renderer->GetDefaultFont()->AddVertsForAlignedText2D(textVerts, rowOutline.GetCenterRight() - Vec2(ROW_NAME_PADDING, 0.f), Vec2(-1.f, 0.f), rowOutline.GetHeight() / 2, row.m_name.ToString());
}



//----------------------------------------------------------------------------------------------------------------------
PerfSection* PerformanceDebugWindow::FindPerfSection(int sectionID)
{
    for (PerfSection& ps : m_perfSections)
    {
        if (ps.m_id == sectionID)
        {
            return &ps;
        }
    }
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
PerfSection* PerformanceDebugWindow::FindPerfSection(Name sectionName)
{
    for (PerfSection& ps : m_perfSections)
    {
        if (ps.m_name == sectionName)
        {
            return &ps;
        }
    }
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
PerfSection& PerformanceDebugWindow::GetOrCreatePerfSection(Name sectionName)
{
    PerfSection* perfSection = FindPerfSection(sectionName);
    if (perfSection)
    {
        return *perfSection;
    }

    int sectionID = CreatePerfSection(sectionName);
    return *FindPerfSection(sectionID);
}



//----------------------------------------------------------------------------------------------------------------------
int PerformanceDebugWindow::CreatePerfSection(Name name)
{
    PerfSection section;
    section.m_name = name;
    static int id = 0;
    section.m_id = id;
    ++id;
    m_perfSections.emplace_back(section);
    return section.m_id;
}



//----------------------------------------------------------------------------------------------------------------------
PerfRow* PerformanceDebugWindow::FindPerfRow(PerfSection& section, int rowID)
{
    if (rowID < (int) section.m_perfRows.size())
    {
        return &section.m_perfRows[rowID];
    }
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
PerfRow* PerformanceDebugWindow::FindPerfRow(PerfSection& section, Name rowName)
{
    for (PerfRow& ps : section.m_perfRows)
    {
        if (ps.m_name == rowName)
        {
            return &ps;
        }
    }
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
PerfRow& PerformanceDebugWindow::GetOrCreatePerfRow(PerfSection& section, Name rowName)
{
    PerfRow* perfRow= FindPerfRow(section, rowName);
    if (perfRow)
    {
        return *perfRow;
    }

    int rowId = CreatePerfRow(section, rowName);
    return section.m_perfRows[rowId];
}



//----------------------------------------------------------------------------------------------------------------------
int PerformanceDebugWindow::CreatePerfRow(PerfSection& section, Name rowName)
{
    PerfRow newRow;
    newRow.m_name = rowName;
    newRow.m_id = (int) section.m_perfRows.size();
    section.m_perfRows.push_back(newRow);
    return newRow.m_id;
}



//----------------------------------------------------------------------------------------------------------------------
void PerformanceDebugWindow::GetGraphOutline(AABB2& out_outline) const
{
    IntVec2 renderResolution = m_window->GetRenderResolution();
    out_outline = AABB2(GRAPH_LEFT_EDGE_PAD, GRAPH_EDGE_PAD, renderResolution.x - GRAPH_EDGE_PAD, renderResolution.y - GRAPH_EDGE_PAD);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 PerformanceDebugWindow::GetItemFrameBounds() const
{
    Vec2 bounds = Vec2(FLT_MAX, 0.f);
    float& earliestStartTime = bounds.x;
    float& latestEndTime = bounds.y;
    for (auto& section : m_perfSections)
    {
        for (auto& row : section.m_perfRows)
        {
            for (auto& item : row.m_perfItemData)
            {
                earliestStartTime = MathUtils::Min(earliestStartTime, (float) item.m_startTime);
                latestEndTime = MathUtils::Max(latestEndTime, (float) item.m_endTime);
            }
        }
    }
    return bounds;
}
