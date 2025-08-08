// Bradley Christensen - 2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include <mutex>
#include <vector>



class Camera;
class System;
class Window;
struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
// THE Job System Debug
//
extern class JobSystemDebug* g_jobSystemDebug;



//----------------------------------------------------------------------------------------------------------------------
struct JobSystemDebugConfig
{
    std::vector<std::string> m_systemSubgraphNames;
};



//----------------------------------------------------------------------------------------------------------------------
struct JobDebugInfo
{
    int     m_threadID      = -1;
    float   m_startTime     = 0.f;
    float   m_endTime       = 0.f;
    System* m_system        = nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
class JobSystemDebug : public EngineSubsystem
{
public:
    
    explicit JobSystemDebug(JobSystemDebugConfig const& config);

    void Startup() override;
    void BeginFrame() override;
    void Update(float deltaSeconds) override;
    void Render() const override;
    void EndFrame() override;
    void Shutdown() override;

    void Log(JobDebugInfo& info);

public:

    bool HandleKeyUp(NamedProperties& args);

private:

    int CountUniqueThreads() const;

protected:

    JobSystemDebugConfig m_config;

    std::mutex m_logMutex;
    std::vector<JobDebugInfo> m_frameDebugInfo;

    Window* m_window = nullptr;
    Camera* m_camera = nullptr;

    // If true, freezes all logging so you can inspect 1 frame indefinitely
    bool m_freezeLog = false;
};
