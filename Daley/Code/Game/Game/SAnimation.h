// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
// Responsible for determining which animation should be playing for each entity, and updates those animation instances
//
class SAnimation : public System
{
public:

    SAnimation(Name name = "Animation", Rgba8 const& debugTint = Rgba8::Cerulean) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
