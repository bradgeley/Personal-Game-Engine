// Bradley Christensen - 2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Renderer/Rgba8.h"
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

};



//----------------------------------------------------------------------------------------------------------------------
// A single point of data, drawn in a PerfRow
struct PerfItemData
{
    float   m_startTime = 0.f;
    float   m_endTime = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
// A row of the performance graph, can be either a thread or something like an ecs system, or group of systems.
struct PerfRow
{
    std::string m_name;
    Rgba8       m_nameTint;
    Rgba8       m_rowDataTint;

    std::vector<PerfItemData> m_perfItemData;
};



//----------------------------------------------------------------------------------------------------------------------
// A section of the performance graph, which encapsulates multiple rows
struct PerfSection
{
    std::string m_name;

protected:

    uint32_t m_id;
    std::vector<PerfRow> m_perfRows;

    friend class PerformanceDebugWindow;
};



//----------------------------------------------------------------------------------------------------------------------
// Data about the frame as a whole
struct PerfFrameData
{
    int     m_frameNumber           = 0;
    float   m_engineFrameStartTime  = 0.f;
    float   m_engineFrameEndTime    = 0.f;
    float   m_actualDeltaSeconds    = 0.f;
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

    void LogData(PerfItemData const& info, int section, int row);
    void UpdatePerfWindowFrameData(PerfFrameData const& info);
    int RegisterSection(std::string const& name);
    int  GetFrameNumber() const;

public:

    bool HandleKeyUp(NamedProperties& args);

private:

    int CountNumRows() const;
    void AddVertsForData(VertexBuffer& vbo, PerfItemData const& debugInfo) const;
    void GetBoundsForData(AABB2& out_jobBounds, PerfItemData const& debugInfo) const;
    void GetBoundsForRow(AABB2& out_rowBounds, PerfItemData const& debugInfo) const;
    void AddVertsForRowText(VertexBuffer& vbo, PerfItemData const& debugInfo) const;

protected:

    PerformanceDebugWindowConfig m_config;
     
    std::mutex m_perfWindowMutex;
    std::vector<PerfSection> m_perfSections;

    PerfFrameData m_perfFrameData;

    Window* m_window = nullptr;
    Camera* m_camera = nullptr;

    // If true, freezes all logging so you can inspect 1 frame indefinitely
    bool m_freezeLog = false;
};