// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SInitView : public System
{
public:

    SInitView(Name name = "InitView", Rgba8 const& debugTint = Rgba8::LightBlue) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
