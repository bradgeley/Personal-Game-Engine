// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SInitView : public System
{
public:

    SInitView(std::string const& name = "Init View") : System(name) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
