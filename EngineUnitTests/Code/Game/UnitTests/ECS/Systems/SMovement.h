#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SMovement : public System
{
public:

	explicit SMovement() : System( "Movement" ) {}

	virtual void Startup() override;
	virtual void Run(SystemContext const& context) override;
};