// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SWorldCollision : public System
{
public:

    SWorldCollision(std::string const& name = "World Collision") : System(name) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
