// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class SDebugRender : public System
{
public:

    SDebugRender(std::string const& name = "DebugRender", Rgba8 const& debugTint = Rgba8::Magenta) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;

protected:

    bool DebugRenderMouseRaycast(NamedProperties& args);
    bool DebugRenderMouseDiscCast(NamedProperties& args);
    bool DebugRenderCostField(NamedProperties& args);
    bool DebugRenderDistanceField(NamedProperties& args);
    bool DebugRenderFlowField(NamedProperties& args);
};
