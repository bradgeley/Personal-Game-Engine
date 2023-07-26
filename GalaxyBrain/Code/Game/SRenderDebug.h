// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRenderDebug : public System
{
public:

    SRenderDebug([[maybe_unused]] std::string const& name = "RenderDebug") {};
    void Startup() override;
    void Run(SystemContext const& context) override;
};
