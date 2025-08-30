// Bradley Christensen - 2022-2025
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"
#include <vector>



class Clock;
class EngineSubsystem;



//----------------------------------------------------------------------------------------------------------------------
// THE Engine
//
extern class Engine* g_engine;



//----------------------------------------------------------------------------------------------------------------------
// Engine //todo: name it!
//
// A group of engine subsystems that run each frame
// The order of registration determines the run order.
//
// Run in forward order:
// - Startup     
// - BeginFrame  
// - Render      
// - EndFrame
//
// Run in backward order:
// - Shutdown
//
class Engine
{
public:

    Engine();
    ~Engine();

    void Startup();
    void BeginFrame();
    void Update();
    void Render() const;
    void EndFrame();
    void Shutdown();

    void RegisterSubsystem(EngineSubsystem* system);

    Clock* GetEngineClock() const;
    
private:
    
    bool m_isActive = false;
    Clock* m_engineClock = nullptr;
    std::vector<EngineSubsystem*> m_subsystems;
};


