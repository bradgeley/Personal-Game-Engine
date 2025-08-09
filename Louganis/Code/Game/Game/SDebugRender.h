// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SDebugRender : public System
{
public:

    SDebugRender(std::string const& name = "Debug Render", Rgba8 const& debugTint = Rgba8::Magenta) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
