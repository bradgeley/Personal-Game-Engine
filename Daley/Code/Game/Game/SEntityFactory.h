// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



struct EntityDef;
struct SpawnInfo;
struct SystemContext;



//----------------------------------------------------------------------------------------------------------------------
class SEntityFactory : public System
{
public:

    SEntityFactory(Name name = "EntityFactory", Rgba8 const& debugTint = Rgba8::Gold) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;

    static EntityID CreateEntityFromDef(SystemContext const& context, EntityDef const* def);
	static EntityID SpawnEntity(SystemContext const& context, SpawnInfo const& spawnInfo); // Usage requires write all dependencies
};
