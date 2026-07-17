// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SFloatingText : public System
{
public:

    SFloatingText(Name name = "FloatingText", Rgba8 const& debugTint = Rgba8::Red) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) const override;
};
