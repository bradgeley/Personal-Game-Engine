// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class SDebugOverlay : public System
{
public:

    SDebugOverlay(Name name = "DebugOverlay", Rgba8 const& debugTint = Rgba8::Magenta) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;

protected:

    static bool ToggleDebugOverlay(NamedProperties& args);
};
