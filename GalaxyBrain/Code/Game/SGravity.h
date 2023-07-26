// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SGravity : public System
{
public:

    SGravity(std::string const& name = "Gravity") : System(name) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
