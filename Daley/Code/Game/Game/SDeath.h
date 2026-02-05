// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SDeath : public System
{
public:

    SDeath(Name name = "Death", Rgba8 const& debugTint = Rgba8::Black) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
