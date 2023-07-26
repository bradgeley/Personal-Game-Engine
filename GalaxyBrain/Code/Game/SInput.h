// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SInput : public System
{
public:

    SInput([[maybe_unused]] std::string const& name = "Input") {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
