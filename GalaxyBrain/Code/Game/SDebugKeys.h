// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SDebugKeys : public System
{
public:

    SDebugKeys([[maybe_unused]] std::string const& name = "DebugKeys") {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override; 
};
