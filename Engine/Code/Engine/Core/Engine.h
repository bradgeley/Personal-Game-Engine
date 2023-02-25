// Bradley Christensen - 2022-2023
#pragma once
#include <vector>



class EngineSubsystem;



//----------------------------------------------------------------------------------------------------------------------
// Engine //todo: name it!
//
// A group of engine subsystems that run each frame
// The order of registration determines the run order.
//
// Startup: forward
// BeginFrame: forward
// Render: forward
// EndFrame: forward
// Shutdown: backward
//
class Engine
{
public:

    Engine() = default;
    ~Engine() = default;

    void Startup();
    void BeginFrame();
    void Update(float deltaSeconds);
    void Render() const;
    void EndFrame();
    void Shutdown();

    void RegisterSubsystem(EngineSubsystem* system);
    
private:
    
    std::vector<EngineSubsystem*> m_subsystems;
};


