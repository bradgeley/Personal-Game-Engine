// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class STime : public System
{
public:

    STime(Name name = "Time", Rgba8 const& debugTint = Rgba8::SkyBlue) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
