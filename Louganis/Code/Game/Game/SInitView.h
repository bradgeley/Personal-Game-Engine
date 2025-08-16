// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SInitView : public System
{
public:

    SInitView(std::string const& name = "InitView", Rgba8 const& debugTint = Rgba8::Red) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
