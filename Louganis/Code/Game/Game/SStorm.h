// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SStorm : public System
{
public:

    SStorm(Name name = "Storm", Rgba8 const& debugTint = Rgba8::DarkViolet) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
