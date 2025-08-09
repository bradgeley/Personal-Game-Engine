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
// THE Job System Debug
//
extern class PerformanceDebugWindow* g_performanceDebugWindow;



//----------------------------------------------------------------------------------------------------------------------
struct PerformanceDebugWindowConfig
{

};



//----------------------------------------------------------------------------------------------------------------------
struct JobDebugInfo
{
    int     m_rowIndex      = -1;
    float   m_startTime     = 0.f;
    float   m_endTime       = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct FrameDebugInfo
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

    void Log(JobDebugInfo const& info);
    void UpdateFrameDebugInfo(FrameDebugInfo const& info);
    int  GetFrameNumber() const;

public:

    bool HandleKeyUp(NamedProperties& args);

private:

    int CountUniqueThreads() const;
    void AddVertsForJob(VertexBuffer& vbo, JobDebugInfo const& debugInfo) const;
    void GetBoundsForJob(AABB2& out_jobBounds, JobDebugInfo const& debugInfo) const;
    void GetBoundsForThread(AABB2& out_threadBounds, JobDebugInfo const& debugInfo) const;
    void AddVertsForThreadText(VertexBuffer& vbo, JobDebugInfo const& debugInfo) const;

protected:

    PerformanceDebugWindowConfig m_config;

    std::mutex m_logMutex;
    std::vector<JobDebugInfo> m_jobDebugLog;

    FrameDebugInfo m_frameDebugInfo;

    Window* m_window = nullptr;
    Camera* m_camera = nullptr;

    // If true, freezes all logging so you can inspect 1 frame indefinitely
    bool m_freezeLog = false;
};
