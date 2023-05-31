// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SDebugKeys : public System
{
public:

    void Run(SystemContext const& context) override;
    
};
