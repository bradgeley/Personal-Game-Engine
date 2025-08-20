// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRenderWorld : public System
{
public:

    SRenderWorld(std::string const& name = "RenderWorld", Rgba8 const& debugTint = Rgba8::Blue) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
