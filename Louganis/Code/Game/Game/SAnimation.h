// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SAnimation : public System
{
public:

    SAnimation(Name name = "Animation", Rgba8 const& debugTint = Rgba8::Cerulean) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
