﻿// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"
#include "Engine/Math/IntVec2.h"



class Chunk;



//----------------------------------------------------------------------------------------------------------------------
class SWorld : public System
{
public:

    SWorld(std::string const& name = "UpdateWorld") : System(name) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
