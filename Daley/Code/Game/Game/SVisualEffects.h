// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SVisualEffects : public System
{
public:

    SVisualEffects(Name name = "VisualEffects", Rgba8 const& debugTint = Rgba8::Purple) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) const override;
};
