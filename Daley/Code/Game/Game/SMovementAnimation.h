// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SMovementAnimation : public System
{
public:

    SMovementAnimation(Name name = "MovementAnimation", Rgba8 const& debugTint = Rgba8::DarkGreen) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
