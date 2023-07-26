// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SCamera : public System
{
public:

    SCamera([[maybe_unused]] std::string const& name = "Camera") {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    
};
