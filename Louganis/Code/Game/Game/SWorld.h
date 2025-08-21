// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"
#include "Engine/Math/IntVec2.h"



class Chunk;
class SCWorld;
struct Vec2;



//----------------------------------------------------------------------------------------------------------------------
class SWorld : public System
{
public:

    SWorld(Name name = "World", Rgba8 const& debugTint = Rgba8::ForestGreen) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;

protected:

    void UpdateLastKnownPlayerLocation(SCWorld& world, Vec2 const& playerLocation) const;
};
