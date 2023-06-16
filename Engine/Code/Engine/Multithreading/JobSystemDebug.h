// Bradley Christensen - 2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"



class Camera;
class Window;



//----------------------------------------------------------------------------------------------------------------------
// THE Job System Debug
//
extern class JobSystemDebug* g_jobSystemDebug;



//----------------------------------------------------------------------------------------------------------------------
struct JobSystemDebugConfig
{
    
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

protected:

    JobSystemDebugConfig m_config;

    Window* m_window = nullptr;
    Camera* m_camera = nullptr;
};
