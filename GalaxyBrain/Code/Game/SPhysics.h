// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SPhysics : public System
{
public:

    SPhysics([[maybe_unused]] std::string const& name = "Physics") {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
