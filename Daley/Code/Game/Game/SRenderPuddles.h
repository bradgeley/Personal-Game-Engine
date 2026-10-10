// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRenderPuddles : public System
{
public:

    SRenderPuddles(Name name = "RenderPuddles", Rgba8 const& debugTint = Rgba8::LightOrange) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;
};
