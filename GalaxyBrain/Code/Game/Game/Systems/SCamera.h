// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SCamera : public System
{
public:

    void Startup() override;
    void Run(SystemContext const& context) override;
    
};
