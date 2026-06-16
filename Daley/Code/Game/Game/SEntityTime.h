// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SEntityTime : public System
{
public:

    SEntityTime(Name name = "EntityTime", Rgba8 const& debugTint = Rgba8::SkyBlue) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) const override; 
};
