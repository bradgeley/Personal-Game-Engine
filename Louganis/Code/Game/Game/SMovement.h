// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SMovement : public System
{
public:

    SMovement(std::string const& name = "Movement", Rgba8 const& debugTint = Rgba8::LightOceanBlue) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
