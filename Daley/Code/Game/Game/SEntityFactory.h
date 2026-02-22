// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



struct EntityDef;
struct SpawnInfo;



//----------------------------------------------------------------------------------------------------------------------
class SEntityFactory : public System
{
public:

    SEntityFactory(Name name = "EntityFactory", Rgba8 const& debugTint = Rgba8::Gold) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;

    static EntityID CreateEntityFromDef(EntityDef const* def);
	static EntityID SpawnEntity(SpawnInfo const& spawnInfo); // Usage requires write all dependencies
};
