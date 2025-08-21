// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/Name.h"
#include <string>



//----------------------------------------------------------------------------------------------------------------------
// Engine Subsystem
//
// Interface of common functions used by all engine subsystems, whether they be audio, renderer, dev console, etc.
//
class EngineSubsystem
{
public:
    
    EngineSubsystem(Name name = "Unnamed Subsystem");
    EngineSubsystem(const EngineSubsystem&) = delete;
    virtual ~EngineSubsystem() = default;
    
    virtual void Startup()                                      {}
    virtual void BeginFrame()                                   {}
    virtual void Update([[maybe_unused]] float deltaSeconds)    {}
    virtual void Render() const                                 {}
    virtual void EndFrame()                                     {}
    virtual void Shutdown()                                     {}

    Name GetName();

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);
    virtual void OnSetEnabled([[maybe_unused]] bool isEnabled)  {}

protected:

    bool m_isEnabled = true;
    Name m_name;
};