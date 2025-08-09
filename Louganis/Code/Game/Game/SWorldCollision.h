// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SWorldCollision : public System
{
public:

    SWorldCollision(std::string const& name = "World Collision", Rgba8 const& debugTint = Rgba8::DarkBlue) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
