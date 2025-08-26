// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SWorldCollision : public System
{
public:

    SWorldCollision(Name name = "WorldCollision", Rgba8 const& debugTint = Rgba8::Firebrick) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
