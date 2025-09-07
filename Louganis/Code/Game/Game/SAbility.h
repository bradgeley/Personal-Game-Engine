// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SAbility : public System
{
public:

    SAbility(Name name = "Ability", Rgba8 const& debugTint = Rgba8::Salmon) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;

protected:
};