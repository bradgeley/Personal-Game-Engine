// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRenderStatusIcons : public System
{
public:

    SRenderStatusIcons(Name name = "RenderStatusIcons", Rgba8 const& debugTint = Rgba8::DarkGray) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;
};
