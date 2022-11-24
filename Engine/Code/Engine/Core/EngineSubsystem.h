// Bradley Christensen - 2022
#pragma once



//----------------------------------------------------------------------------------------------------------------------
// Engine Subsystem
//
// Interface of common functions used by all engine subsystems, whether they be audio, renderer, dev console, etc.
//
class EngineSubsystem
{
public:
    
    virtual ~EngineSubsystem() = default;
    
    virtual void Startup()                      {}
    virtual void BeginFrame()                   {}
    virtual void Update(float deltaSeconds)     {}
    virtual void Render()                       {}
    virtual void EndFrame()                     {}
    virtual void Shutdown()                     {}

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);
    virtual void OnSetEnabled(bool isEnabled)   {}

private:

    bool m_isEnabled = true;
};