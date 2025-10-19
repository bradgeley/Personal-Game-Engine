// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class SDebugRender : public System
{
public:

    SDebugRender(Name name = "DebugRender", Rgba8 const& debugTint = Rgba8::Magenta) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;

protected:

    static bool DebugRenderMouseRaycast(NamedProperties& args);
    static bool DebugRenderMouseDiscCast(NamedProperties& args);
    static bool DebugRenderCostField(NamedProperties& args);
    static bool DebugRenderDistanceField(NamedProperties& args);
    static bool DebugRenderFlowField(NamedProperties& args);
    static bool DebugRenderSolidTiles(NamedProperties& args);
    static bool DebugRenderCollision(NamedProperties& args);
    static bool DebugRenderLighting(NamedProperties& args);
    static bool DebugRenderGrid(NamedProperties& args);
    static bool DebugRenderTileIDs(NamedProperties& args);
};
