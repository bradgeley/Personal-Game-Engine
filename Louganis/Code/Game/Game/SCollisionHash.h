// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SCollisionHash : public System
{
public:

    SCollisionHash(Name name = "CollisionHash", Rgba8 const& debugTint = Rgba8::Maroon) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
