// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SInput : public System
{
public:

    SInput(std::string const& name = "Input", Rgba8 const& debugTint = Rgba8::Lime) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
