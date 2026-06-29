// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRenderPauseMenu : public System
{
public:

    SRenderPauseMenu(Name name = "RenderPauseMenu", Rgba8 const& debugTint = Rgba8::Black) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;
};
