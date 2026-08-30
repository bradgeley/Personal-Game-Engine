// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRunModifier : public System
{
public:

    SRunModifier(Name name = "RunModifier", Rgba8 const& debugTint = Rgba8::Red) : System(name, debugTint) {};
    virtual void Startup() override;
    virtual void Run(SystemContext const& context) const override;
};
