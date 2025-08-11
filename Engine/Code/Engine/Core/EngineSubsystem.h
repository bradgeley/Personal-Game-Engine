// Bradley Christensen - 2022-2023
#pragma once
#include <string>



//----------------------------------------------------------------------------------------------------------------------
// Engine Subsystem
//
// Interface of common functions used by all engine subsystems, whether they be audio, renderer, dev console, etc.
//
class EngineSubsystem
{
public:
    
    EngineSubsystem(std::string const& name = "Unnamed Subsystem");
    EngineSubsystem(const EngineSubsystem&) = delete;
    virtual ~EngineSubsystem() = default;
    
    virtual void Startup()                                      {}
    virtual void BeginFrame()                                   {}
    virtual void Update([[maybe_unused]] float deltaSeconds)    {}
    virtual void Render() const                                 {}
    virtual void EndFrame()                                     {}
    virtual void Shutdown()                                     {}

    std::string const& GetName();

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);
    virtual void OnSetEnabled([[maybe_unused]] bool isEnabled)  {}

protected:

    bool m_isEnabled = true;
    std::string m_name;
};