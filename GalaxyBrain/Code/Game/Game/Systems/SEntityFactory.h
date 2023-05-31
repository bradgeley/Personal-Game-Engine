// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



struct EntityDef;



//----------------------------------------------------------------------------------------------------------------------
class SEntityFactory : public System
{
public:

    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;

private:

    EntityID CreateEntityFromDef(EntityDef const* def) const;
    void LoadFromXml(char const* filename) const;
};
