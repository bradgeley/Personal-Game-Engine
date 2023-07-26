// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SMovement : public System
{
public:

    SMovement([[maybe_unused]] std::string const& name = "Movement") {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
