// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SCollision : public System
{
public:

    SCollision(std::string const& name = "Collision", Rgba8 const& debugTint = Rgba8::LightBlue) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
