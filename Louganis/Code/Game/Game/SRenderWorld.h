// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRenderWorld : public System
{
public:

    SRenderWorld(std::string const& name = "RenderWorld") : System(name) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
