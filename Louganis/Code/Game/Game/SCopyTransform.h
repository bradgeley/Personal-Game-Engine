// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SCopyTransform : public System
{
public:

    SCopyTransform(Name name = "CopyTransform", Rgba8 const& debugTint = Rgba8::White) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
