// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRenderStorm : public System
{
public:

    SRenderStorm(Name name = "RenderStorm", Rgba8 const& debugTint = Rgba8::DarkViolet) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
