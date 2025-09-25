// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



enum class TimeOfDay;



//----------------------------------------------------------------------------------------------------------------------
class SRenderTimeOfDay : public System
{
public:

    SRenderTimeOfDay(Name name = "RenderTimeOfDay", Rgba8 const& debugTint = Rgba8::DarkBlue) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
