// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



enum class TimeOfDay;



//----------------------------------------------------------------------------------------------------------------------
class SRenderWorld : public System
{
public:

    SRenderWorld(Name name = "RenderWorld", Rgba8 const& debugTint = Rgba8::Blue) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;
};
