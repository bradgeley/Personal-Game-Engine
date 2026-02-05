// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



enum class TimeOfDay;



//----------------------------------------------------------------------------------------------------------------------
class SRenderWorld : public System
{
public:

    SRenderWorld(Name name = "RenderWorld", Rgba8 const& debugTint = Rgba8::Blue) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
