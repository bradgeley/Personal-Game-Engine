// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SCamera : public System
{
public:

    SCamera(Name name = "Camera", Rgba8 const& debugTint = Rgba8::SkyBlue) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    
};
