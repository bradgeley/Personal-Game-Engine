// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SPhysics : public System
{
public:

	explicit SPhysics() : System( "Physics" ) {}

	virtual void Startup() override;
	virtual void Run(SystemContext const& context) override;
};