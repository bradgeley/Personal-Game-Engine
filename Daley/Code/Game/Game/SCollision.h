// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SCollision : public System
{
public:

    SCollision(Name name = "Collision", Rgba8 const& debugTint = Rgba8::Red) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
