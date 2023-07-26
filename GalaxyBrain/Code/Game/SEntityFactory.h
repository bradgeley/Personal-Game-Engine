﻿// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



struct EntityDef;



//----------------------------------------------------------------------------------------------------------------------
class SEntityFactory : public System
{
public:

    SEntityFactory([[maybe_unused]] std::string const& name = "EntityFactory") {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;

    static EntityID CreateEntityFromDef(EntityDef const* def);

private:

    void LoadFromXml(char const* filename) const;
};
