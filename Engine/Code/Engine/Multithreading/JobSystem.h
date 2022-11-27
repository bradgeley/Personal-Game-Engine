// Bradley Christensen - 2022
#pragma once
#include "Engine/Core/EngineSubsystem.h"



//----------------------------------------------------------------------------------------------------------------------
// Job System
//
// Owns worker threads and runs jobs.
//
class JobSystem : EngineSubsystem
{
    virtual void Startup() override;
    virtual void BeginFrame() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() override;
    virtual void EndFrame() override;
    virtual void Shutdown() override;
};