// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SCopyTransform : public System
{
public:

    SCopyTransform(std::string const& name = "CopyTransform", Rgba8 const& debugTint = Rgba8::LightRed) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
