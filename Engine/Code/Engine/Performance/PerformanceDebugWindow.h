// Bradley Christensen - 2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Renderer/VertexBuffer.h"
#include <mutex>
#include <vector>



class Camera;
class System;
class VertexBuffer;
class Window;
struct NamedProperties;
struct AABB2;



//----------------------------------------------------------------------------------------------------------------------
// THE Performance Debug Window (System)
//
extern class PerformanceDebugWindow* g_performanceDebugWindow;



//----------------------------------------------------------------------------------------------------------------------
struct PerformanceDebugWindowConfig
{
    bool  m_startOpen        = false;
    Rgba8 m_graphOutlineTint = Rgba8::Black;
};



//----------------------------------------------------------------------------------------------------------------------
// A single point of data, drawn in a PerfRow
struct PerfItemData
{
    double   m_startTime     = 0.f;
    double   m_endTime       = 0.f;
    Rgba8    m_tint          = Rgba8::White;
};



//----------------------------------------------------------------------------------------------------------------------
// A row of the performance graph, can be either a thread or something like an ecs system, or group of systems.
struct PerfRow
{
    Name m_name                 = "Unnamed Perf Row";

protected:

    int                         m_id;
    std::vector<PerfItemData>   m_perfItemData;

    friend class PerformanceDebugWindow;
};



//----------------------------------------------------------------------------------------------------------------------
// A section of the performance graph, which encapsulates multiple rows
struct PerfSection
{
    Name m_name             = "Unnamed Perf Section";

protected:

    int                     m_id;
    std::vector<PerfRow>    m_perfRows;

    friend class PerformanceDebugWindow;
};



//----------------------------------------------------------------------------------------------------------------------
// Data about the frame as a whole
struct PerfFrameData
{
    int      m_frameNumber           = 0;
    double   m_engineFrameStartTime  = 0.f;
    double   m_engineFrameEndTime    = 0.f;
    double   m_actualDeltaSeconds    = 0.f;
    bool     m_normalizeFrameTime    = true; // if true, the shown graph nodes will fill the available space
};



//----------------------------------------------------------------------------------------------------------------------
class PerformanceDebugWindow : public EngineSubsystem
{
public:

    explicit PerformanceDebugWindow(PerformanceDebugWindowConfig const& config);

    void Startup() override;
    void BeginFrame() override;
    void Update(float deltaSeconds) override;
    void Render() const override;
    void EndFrame() override;
    void Shutdown() override;

    bool OpenWindow();
    bool CloseWindow();

    void LogItem(PerfItemData const& item, int sectionID, int rowID);
    void LogItem(PerfItemData const& item, Name sectionName, Name rowName);
    void UpdateFrameData(PerfFrameData const& info);
    int GetFrameNumber();

    int GetOrCreateSectionID(Name sectionName);
    int GetOrCreateRowID(int sectionID, Name rowName);

    void EngineFrameCompleted();

public:

    bool HandleMainWindowKeyUp(NamedProperties& args);
    bool HandlePerfWindowKeyUp(NamedProperties& args);
    bool WindowSizeChanged(NamedProperties& args);
    bool HandleWindowQuit(NamedProperties& args);

private:

    // Internal functions, not mutex guarded
    int CountNumRows() const;
    int CountNumRowsBeforeSection(int sectionID) const;

    void AddUntexturedVertsForSection(VertexBuffer& untexturedVerts, PerfSection const& section);
    void AddTextVertsForSection(VertexBuffer& textVerts, PerfSection const& section);

    void AddUntexturedVertsForRow(VertexBuffer& untexturedVerts, PerfSection const& section, PerfRow const& row);
    void AddTextVertsForRow(VertexBuffer& textVerts, PerfSection const& section, PerfRow const& row);

    PerfSection* FindPerfSection(int sectionID);
    PerfSection* FindPerfSection(Name sectionName);
    PerfSection& GetOrCreatePerfSection(Name sectionName);
    int CreatePerfSection(Name name);

    PerfRow* FindPerfRow(PerfSection& section, int rowID);
    PerfRow* FindPerfRow(PerfSection& section, Name rowName);
    PerfRow& GetOrCreatePerfRow(PerfSection& section, Name rowName);
    int CreatePerfRow(PerfSection& section, Name rowName);

    void GetGraphOutline(AABB2& out_outline) const;
    Vec2 GetItemFrameBounds() const;

protected:

    std::mutex m_mutex;

    PerformanceDebugWindowConfig m_config;

    std::vector<PerfSection> m_perfSections;

    PerfFrameData m_perfFrameData;

    Window* m_window = nullptr;
    Camera* m_camera = nullptr;

    VertexBuffer* m_untexturedVBO;
    VertexBuffer* m_textVBO;

    // If true, freezes all logging so you can inspect 1 frame indefinitely
    bool m_freezeLog = false;
};