// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SPreRender : public System
{
public:

    SPreRender(std::string const& name = "PreRender") : System(name) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
