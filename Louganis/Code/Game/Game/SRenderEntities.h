// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRenderEntities : public System
{
public:

    SRenderEntities(std::string const& name = "RenderEntities", Rgba8 const& debugTint = Rgba8::Green) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
