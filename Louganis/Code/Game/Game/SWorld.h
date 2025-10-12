// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"
#include "Engine/Math/IntVec2.h"



class Chunk;
class SCWorld;
struct NamedProperties;
struct Vec2;


//----------------------------------------------------------------------------------------------------------------------
class SWorld : public System
{
public:

    SWorld(Name name = "World", Rgba8 const& debugTint = Rgba8::ForestGreen) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
	void EndFrame() override;
    void Shutdown() override;

protected:

    void UpdateLastKnownPlayerLocation(SCWorld& world, Vec2 const& playerLocation) const;
};
