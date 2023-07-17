// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SCollision : public System
{
public:

    void Startup() override;
    void Run(SystemContext const& context) override;

    void DetachEntities(EntityID a, EntityID b) const;
};
