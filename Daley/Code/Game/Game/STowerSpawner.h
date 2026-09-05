// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



struct NamedProperties;
struct SCEntityFactory;
struct TowerPlacementRequest;
class SCWorld;



//----------------------------------------------------------------------------------------------------------------------
enum class TowerPlacementResult
{
    Success,
    Blocked,
    BlocksPath,
    CannotAfford,
};



//----------------------------------------------------------------------------------------------------------------------
class STowerSpawner : public System
{
public:

    STowerSpawner(Name name = "TowerSpawner", Rgba8 const& debugTint = Rgba8::Gray) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;

    TowerPlacementResult CanPlaceTower(TowerPlacementRequest const& info, SCWorld const& world) const;
    bool PlaceTowerInWorld(TowerPlacementRequest const& placementInfo, SCWorld& world) const;
	bool WillChangePathSolidness(TowerPlacementRequest const& placementInfo, SCWorld const& world) const;

    static bool FillMapWithTower(NamedProperties& properties);
};
