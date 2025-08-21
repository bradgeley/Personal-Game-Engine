// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SMovement : public System
{
public:

    SMovement(Name name = "Movement", Rgba8 const& debugTint = Rgba8::Green) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
