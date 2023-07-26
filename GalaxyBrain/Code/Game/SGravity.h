// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SGravity : public System
{
public:

    SGravity([[maybe_unused]] std::string const& name = "Gravity") {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
