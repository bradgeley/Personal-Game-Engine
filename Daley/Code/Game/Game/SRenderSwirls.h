// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRenderSwirls : public System
{
public:

    SRenderSwirls(Name name = "RenderSwirls", Rgba8 const& debugTint = Rgba8::DarkViolet) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) const override;
    void Shutdown() const override;
};
