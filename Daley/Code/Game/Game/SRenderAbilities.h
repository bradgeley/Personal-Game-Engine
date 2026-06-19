// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRenderAbilities : public System
{
public:

    SRenderAbilities(Name name = "RenderAbilities", Rgba8 const& debugTint = Rgba8::DarkBlue) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;
};
