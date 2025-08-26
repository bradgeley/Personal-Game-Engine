﻿// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRenderEntities : public System
{
public:

    SRenderEntities(Name name = "RenderEntities", Rgba8 const& debugTint = Rgba8::Cerulean) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
