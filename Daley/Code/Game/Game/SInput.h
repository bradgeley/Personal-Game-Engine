// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SInput : public System
{
public:

    SInput(Name name = "Input", Rgba8 const& debugTint = Rgba8::Lime) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
