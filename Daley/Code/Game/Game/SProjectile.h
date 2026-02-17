// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SProjectile : public System
{
public:

    SProjectile(Name name = "Projectile", Rgba8 const& debugTint = Rgba8::Yellow) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
