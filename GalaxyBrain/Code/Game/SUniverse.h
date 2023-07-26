// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SUniverse : public System
{
public:

    SUniverse([[maybe_unused]] std::string const& name = "Universe") {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
