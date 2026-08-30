// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRenderModSelection : public System
{
public:

    SRenderModSelection(Name name = "RenderModSelection", Rgba8 const& debugTint = Rgba8::Blue) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;
};
