// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRenderHealthBars : public System
{
public:

    SRenderHealthBars(Name name = "RenderHealthBars", Rgba8 const& debugTint = Rgba8::Red) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;
};
