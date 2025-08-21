// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



struct EntityDef;



//----------------------------------------------------------------------------------------------------------------------
class SEntityFactory : public System
{
public:

    SEntityFactory(Name name = "EntityFactory", Rgba8 const& debugTint = Rgba8::Gold) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;

    static EntityID CreateEntityFromDef(EntityDef const* def);

private:

    void LoadFromXml(char const* filename) const;
};
