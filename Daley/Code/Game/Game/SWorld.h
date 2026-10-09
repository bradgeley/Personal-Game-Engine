// Bradley Christensen - 2022-2026
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
    virtual void Startup() override;
	virtual void Run(SystemContext const& context) const override;
    virtual void Shutdown() const override;
	virtual void EndFrame() const override;

protected:

	static bool GenerateMap(NamedProperties& params);
};
