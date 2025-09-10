﻿// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SAIController : public System
{
public:

    SAIController(Name name = "AIController", Rgba8 const& debugTint = Rgba8::Cyan) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
