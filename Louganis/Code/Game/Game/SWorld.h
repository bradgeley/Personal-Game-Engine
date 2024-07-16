// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SWorld : public System
{
public:

    SWorld(std::string const& name = "UpdateWorld") : System(name) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
