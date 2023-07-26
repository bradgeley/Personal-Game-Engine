// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SInput : public System
{
public:

    SInput(std::string const& name = "Input") : System(name) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
