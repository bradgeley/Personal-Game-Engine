// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



enum class TimeOfDay;



//----------------------------------------------------------------------------------------------------------------------
class SLighting : public System
{
public:

    SLighting(Name name = "Lighting", Rgba8 const& debugTint = Rgba8::LightYellow) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
