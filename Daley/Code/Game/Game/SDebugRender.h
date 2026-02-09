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

    static bool DebugRenderGrid(NamedProperties& args);
    static bool DebugTileTags(NamedProperties& args); // Used to visualize tile tags
};
