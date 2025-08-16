// Bradley Christensen - 2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SPreventativeCollision : public System
{
public:

    SPreventativeCollision(std::string const& name = "PreventativeCollision", Rgba8 const& debugTint = Rgba8::Orange) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};