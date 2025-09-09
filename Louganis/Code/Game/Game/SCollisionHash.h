// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SCollisionHash : public System
{
public:

    SCollisionHash(Name name = "CollisionHash", Rgba8 const& debugTint = Rgba8::Maroon) : System(name, debugTint) {};
    virtual void Startup() override;
	virtual void PreRun() override;
    virtual void Run(SystemContext const& context) override;
	virtual void PostRun() override;
};
