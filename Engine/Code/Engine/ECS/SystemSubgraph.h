// Bradley Christensen - 2022-2025
#pragma once
#include <vector>



class System;



typedef size_t SystemSubgraphID;



//----------------------------------------------------------------------------------------------------------------------
// System Sub-graph
//
// A list of systems that always run together, in priority order between each other - or in a job graph with auto multi-
// threading enabled. Primarily useful for subgraphs that need to run on a time-step multiple times per frame, like physics.
//
class SystemSubgraph
{
    friend class SystemScheduler;

public:

    void Startup() const;
    void Shutdown() const;
    void Cleanup();
     
public:

    std::vector<System*> m_systems;
    
    float m_timeStep            = 0.f; // if time step < SYSTEM_MIN_TIME_STEP, run once only, 0.002 means run this subgraph every 0.002 seconds of accumulated time

protected:
    
    float m_accumulatedTime     = 0.f;
};
