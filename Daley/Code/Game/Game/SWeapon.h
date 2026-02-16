// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SWeapon : public System
{
public:

    SWeapon(Name name = "Weapon", Rgba8 const& debugTint = Rgba8::Crimson) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
