// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SLifetime : public System
{
public:

    SLifetime(Name name = "Lifetime", Rgba8 const& debugTint = Rgba8::MediumSlateBlue) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};