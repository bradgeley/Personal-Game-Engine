// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SGoal : public System
{
public:

    SGoal(Name name = "Goal", Rgba8 const& debugTint = Rgba8::White) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
