// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SFlowField : public System
{
public:

    SFlowField(std::string const& name = "FlowField") : System(name) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    
};
