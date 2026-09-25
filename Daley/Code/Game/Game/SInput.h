// Bradley Christensen - 2022-2026
#pragma once
#include "TowerPlacementRequest.h"
#include "Engine/ECS/System.h"



class SCWorld;
struct PlaceableTower;
struct RunData;
struct Vec2;



//----------------------------------------------------------------------------------------------------------------------
class SInput : public System
{
public:

    SInput(Name name = "Input", Rgba8 const& debugTint = Rgba8::Lime) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) const override;

	static bool CanAffordTower(PlaceableTower const& tower, RunData const& runData);
	static bool CanAffordSwirl(PlaceableTower const& tower, RunData const& runData);
    static TowerPlacementRequest MakeTowerPlacementRequest(Name towerEntityName, Name flavorName, Vec2 const& worldPos, SCWorld const& world, bool isGenerated = false, float cost = 0.f, bool canAfford = true);
    static TowerSwirlRequest MakeTowerSwirlRequest(EntityID towerEntityID, Name flavorName, Vec2 const& worldPos, float cost = 0.f, bool canAfford = true);
};
