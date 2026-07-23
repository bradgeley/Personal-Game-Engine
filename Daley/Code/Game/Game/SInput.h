// Bradley Christensen - 2022-2026
#pragma once
#include "TowerPlacementInfo.h"
#include "Engine/ECS/System.h"



class SCWorld;
struct Vec2;



//----------------------------------------------------------------------------------------------------------------------
class SInput : public System
{
public:

    SInput(Name name = "Input", Rgba8 const& debugTint = Rgba8::Lime) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) const override;

    static TowerPlacementInfo MakeTowerPlacementInfo(Name towerDefName, Vec2 const& worldPos, SCWorld const& world);
};
